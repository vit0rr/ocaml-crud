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
      Lwt.return
        (Dream.response ~status:`Unauthorized
           (Printf.sprintf {|{"error": "%s"}|} err))

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
         get "/" (fun _ -> html "Hello, World");
         post "/users" (fun request ->
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
                 Dream.json
                   (Printf.sprintf {|{"id": "%s", "email": "%s"}|} user.id
                      user.email)
             | Error err ->
                 let error_msg = Caqti_error.show err in
                 Printf.printf "Debug - Full error message: %s\n%!" error_msg;
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
                 Lwt.return
                   (Dream.response ~status
                      (Printf.sprintf {|{"error": "%s"}|} message)));
         delete "/users/:id" (fun request ->
             let id = param request "id" in
             let%lwt delete_result = User.delete_user id in
             match delete_result with
             | Ok id ->
                 Dream.json
                   (Printf.sprintf {|{"id": "%s", "message": "User deleted"}|}
                      id)
             | Error err ->
                 Lwt.return
                   (Dream.response ~status:`Internal_Server_Error
                      (Printf.sprintf "Error deleting user: %s"
                         (Caqti_error.show err))));
         post "/login" (fun request ->
             let%lwt body = Dream.body request in
             try
               let json = Yojson.Safe.from_string body in
               let email =
                 match json |> Yojson.Safe.Util.member "email" with
                 | `String s -> s
                 | `Null ->
                     Printf.printf "Email is null\n%!";
                     raise
                       (Yojson.Safe.Util.Type_error ("Email is required", json))
                 | _ ->
                     Printf.printf "Email is invalid type\n%!";
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
                   Lwt.return
                     (Dream.response ~code:200
                        (Printf.sprintf
                           {|{"id": "%s", "email": "%s", "token": "%s"}|}
                           user.id user.email (Jose.Jwt.to_string token)))
               | Error err ->
                   Lwt.return
                     (Dream.response ~status:`Unauthorized
                        (Printf.sprintf {|{"error": "%s"}|} err))
             with
             | Yojson.Safe.Util.Type_error (msg, _) ->
                 Printf.printf "Type error: %s\n%!" msg;
                 Lwt.return
                   (Dream.response ~status:`Bad_Request
                      (Printf.sprintf {|{"error": "%s"}|} msg))
             | Yojson.Json_error msg ->
                 Printf.printf "JSON parse error: %s\n%!" msg;
                 Lwt.return
                   (Dream.response ~status:`Bad_Request
                      (Printf.sprintf {|{"error": "Invalid JSON: %s"}|} msg)));
         get "/verify-token"
           (protect_route (fun request ->
                match verify_auth_token request with
                | Ok user_id ->
                    Dream.json (Printf.sprintf {|{"user_id": "%s"}|} user_id)
                | Error _ -> Dream.json {|{"error": "Invalid token"}|}));
       ]
