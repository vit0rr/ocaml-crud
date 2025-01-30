open Dream
open Ocaml_crud

let verify_auth_token request =
  match Dream.header request "Authorization" with
  | None -> Error "No authorization header"
  | Some header -> (
      match String.split_on_char ' ' header with
      | [ "Bearer"; token ] -> User.verify_token token
      | _ -> Error "Invalid authorization header format")

let protect_route inner_handler request =
  match verify_auth_token request with
  | Ok _ -> inner_handler request
  | Error err ->
      Dream.json ~status:`Unauthorized (Printf.sprintf {|{"error": "%s"}|} err)

let cors_middleware inner_handler request =
  let cors_headers =
    [
      ("Access-Control-Allow-Origin", "*");
      ("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
      ("Access-Control-Allow-Headers", "Content-Type, Authorization");
      ("Access-Control-Allow-Credentials", "true");
      ("Content-Type", "application/json");
    ]
  in
  match Dream.method_ request with
  | `OPTIONS -> Lwt.return (Dream.response ~headers:cors_headers ~code:204 "")
  | _ ->
      let%lwt response = inner_handler request in
      List.iter
        (fun (header, value) -> Dream.add_header response header value)
        cors_headers;
      Lwt.return response

(* 
TODO: Not sure if this is the best aproach in OCaml
ref.: https://stackoverflow.com/questions/8373460/substring-check-in-ocaml  
*)
let contains s1 s2 =
  let re = Str.regexp_string s2 in
  try
    ignore (Str.search_forward re s1 0);
    true
  with Not_found -> false

let () =
  run @@ cors_middleware
  @@ router
       [
         get "/verify-token"
           (protect_route (fun request ->
                Printf.printf "[GET /verify-token] Request received\n%!";
                match verify_auth_token request with
                | Ok user_id ->
                    Printf.printf "[GET /verify-token] Verified user_id: %s\n%!"
                      user_id;
                    Dream.json (Printf.sprintf {|{"user_id": "%s"}|} user_id)
                | Error err ->
                    Printf.printf "[GET /verify-token] Error: %s\n%!" err;
                    Dream.json ~status:`Unauthorized
                      (Printf.sprintf {|{"error": "%s"}|} err)));
         post "/users" (fun request ->
             Printf.printf "[POST /users] Request received\n%!";
             let%lwt body = Dream.body request in
             let json = Yojson.Safe.from_string body in
             let email =
               json
               |> Yojson.Safe.Util.member "email"
               |> Yojson.Safe.Util.to_string
             in
             let password =
               json
               |> Yojson.Safe.Util.member "password"
               |> Yojson.Safe.Util.to_string
             in
             let%lwt user_result = User.create_user email password in
             match user_result with
             | Ok user ->
                 Printf.printf "[POST /users] User created: %s\n%!" user.id;
                 Dream.json
                   (Printf.sprintf {|{"id": "%s", "email": "%s"}|} user.id
                      user.email)
             | Error err ->
                 let error_msg = Caqti_error.show err in
                 Printf.printf "[POST /users] Error: %s\n%!" error_msg;
                 let status, message =
                   match error_msg with
                   | msg
                     when contains msg "duplicate key value"
                          && contains msg "users_email_key" ->
                       (`Conflict, "An account with this email already exists")
                   | _ ->
                       ( `Internal_Server_Error,
                         "An error occurred while creating the user" )
                 in
                 Dream.json ~status (Printf.sprintf {|{"error": "%s"}|} message));
         post "/login" (fun request ->
             Printf.printf "[POST /login] Request received\n%!";
             let%lwt body = Dream.body request in
             try
               let json = Yojson.Safe.from_string body in
               let email =
                 match json |> Yojson.Safe.Util.member "email" with
                 | `String s -> s
                 | `Null ->
                     raise
                       (Yojson.Safe.Util.Type_error ("Email is required", json))
                 | _ ->
                     raise
                       (Yojson.Safe.Util.Type_error
                          ("Email must be a string", json))
               in
               let password =
                 match json |> Yojson.Safe.Util.member "password" with
                 | `String s -> s
                 | `Null ->
                     raise
                       (Yojson.Safe.Util.Type_error
                          ("Password is required", json))
                 | _ ->
                     raise
                       (Yojson.Safe.Util.Type_error
                          ("Password must be a string", json))
               in
               let%lwt login_result = User.login email password in
               match login_result with
               | Ok (user, token) ->
                   Printf.printf "[POST /login] User logged in: %s\n%!" user.id;
                   Dream.json
                     (Printf.sprintf
                        {|{"id": "%s", "email": "%s", "token": "%s"}|} user.id
                        user.email (Jose.Jwt.to_string token))
               | Error err ->
                   Printf.printf "[POST /login] Error: %s\n%!" err;
                   Dream.json ~status:`Unauthorized
                     (Printf.sprintf {|{"error": "%s"}|} err)
             with
             | Yojson.Safe.Util.Type_error (msg, _) ->
                 Printf.printf "[POST /login] Type error: %s\n%!" msg;
                 Dream.json ~status:`Bad_Request
                   (Printf.sprintf {|{"error": "%s"}|} msg)
             | Yojson.Json_error msg ->
                 Printf.printf "[POST /login] JSON parse error: %s\n%!" msg;
                 Dream.json ~status:`Bad_Request
                   (Printf.sprintf {|{"error": "Invalid JSON: %s"}|} msg));
         post "/tasks"
           (protect_route (fun request ->
                Printf.printf "[POST /tasks] Request received\n%!";
                let%lwt body = Dream.body request in
                let json = Yojson.Safe.from_string body in
                let task =
                  json
                  |> Yojson.Safe.Util.member "task"
                  |> Yojson.Safe.Util.to_string
                in
                match verify_auth_token request with
                | Ok user_id -> (
                    Printf.printf "[POST /tasks] User: %s\n%!" user_id;
                    let%lwt task_result = Tasks.create_task task user_id in
                    match task_result with
                    | Ok task ->
                        Printf.printf "[POST /tasks] Task created: %s\n%!"
                          task.id;
                        Dream.json
                          (Printf.sprintf
                             {|{"id": "%s", "task": "%s", "user_id": "%s"}|}
                             task.id task.task task.user_id)
                    | Error err ->
                        let error_msg = Caqti_error.show err in
                        Printf.printf "[POST /tasks] Error: %s\n%!" error_msg;
                        Dream.json ~status:`Internal_Server_Error
                          (Printf.sprintf {|{"error": "%s"}|} error_msg))
                | Error err ->
                    Printf.printf "[POST /tasks] Auth error: %s\n%!" err;
                    Dream.json ~status:`Unauthorized
                      (Printf.sprintf {|{"error": "%s"}|} err)));
         get "/tasks"
           (protect_route (fun request ->
                Printf.printf "[GET /tasks] Request received\n%!";
                match verify_auth_token request with
                | Ok user_id -> (
                    Printf.printf "[GET /tasks] User: %s\n%!" user_id;
                    let%lwt tasks_result = Tasks.get_tasks user_id in
                    match tasks_result with
                    | Ok tasks ->
                        Printf.printf "[GET /tasks] Retrieved %d tasks\n%!"
                          (List.length tasks);
                        let tasks_json =
                          tasks
                          |> List.map (fun (task : Tasks.t) ->
                                 Printf.sprintf
                                   {|{"id": "%s", "task": "%s", "user_id": "%s"}|}
                                   task.id task.task task.user_id)
                          |> String.concat ", "
                        in
                        Dream.json (Printf.sprintf "[%s]" tasks_json)
                    | Error err ->
                        let error_msg = Caqti_error.show err in
                        Printf.printf "[GET /tasks] Error: %s\n%!" error_msg;
                        Dream.json ~status:`Internal_Server_Error
                          (Printf.sprintf {|{"error": "%s"}|} error_msg))
                | Error err ->
                    Printf.printf "[GET /tasks] Auth error: %s\n%!" err;
                    Dream.json ~status:`Unauthorized
                      (Printf.sprintf {|{"error": "%s"}|} err)));
         delete "/tasks/:id"
           (protect_route (fun request ->
                let id = param request "id" in
                Printf.printf "[DELETE /tasks/%s] Request received\n%!" id;
                match verify_auth_token request with
                | Ok user_id -> (
                    Printf.printf "[DELETE /tasks/%s] User: %s\n%!" id user_id;
                    let%lwt delete_result = Tasks.delete_task id user_id in
                    match delete_result with
                    | Ok () ->
                        Printf.printf "[DELETE /tasks/%s] Task deleted\n%!" id;
                        Dream.json {|{"message": "Task deleted successfully"}|}
                    | Error err ->
                        let error_msg = Caqti_error.show err in
                        Printf.printf "[DELETE /tasks/%s] Error: %s\n%!" id
                          error_msg;
                        Dream.json ~status:`Internal_Server_Error
                          (Printf.sprintf {|{"error": "%s"}|} error_msg))
                | Error err ->
                    Printf.printf "[DELETE /tasks/%s] Auth error: %s\n%!" id err;
                    Dream.json ~status:`Unauthorized
                      (Printf.sprintf {|{"error": "%s"}|} err)));
         put "/tasks/:id"
           (protect_route (fun request ->
                let id = param request "id" in
                Printf.printf "[PUT /tasks/%s] Request received\n%!" id;
                let%lwt body = Dream.body request in
                let json = Yojson.Safe.from_string body in
                let task =
                  json
                  |> Yojson.Safe.Util.member "task"
                  |> Yojson.Safe.Util.to_string
                in
                match verify_auth_token request with
                | Ok user_id -> (
                    Printf.printf "[PUT /tasks/%s] User: %s\n%!" id user_id;
                    let%lwt update_result = Tasks.update_task id task user_id in
                    match update_result with
                    | Ok () ->
                        Printf.printf "[PUT /tasks/%s] Task updated\n%!" id;
                        Dream.json {|{"message": "Task updated successfully"}|}
                    | Error err ->
                        let error_msg = Caqti_error.show err in
                        Printf.printf "[PUT /tasks/%s] Error: %s\n%!" id
                          error_msg;
                        Dream.json ~status:`Internal_Server_Error
                          (Printf.sprintf {|{"error": "%s"}|} error_msg))
                | Error err ->
                    Printf.printf "[PUT /tasks/%s] Auth error: %s\n%!" id err;
                    Dream.json ~status:`Unauthorized
                      (Printf.sprintf {|{"error": "%s"}|} err)));
       ]
