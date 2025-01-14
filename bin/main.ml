open Dream
open Ocaml_crud

let () =
  run
  @@ router
       [
         get "/" (fun _ -> html "Hello, World");
         get "/users/:id" (fun request ->
             let id = param request "id" in
             let%lwt user_result = User.get_user_by_id id in
             match user_result with
             | Ok user ->
                 Dream.json
                   (Printf.sprintf {|{"id": "%s", "name": "%s", "email": "%s"}|}
                      user.id user.name user.email)
             | Error err ->
                 Lwt.return
                   (Dream.response ~status:`Internal_Server_Error
                      (Printf.sprintf "Error getting user: %s"
                         (Caqti_error.show err))));
         post "/users" (fun request ->
             let%lwt body = Dream.body request in
             let json = Yojson.Safe.from_string body in
             let name =
               json
               |> Yojson.Safe.Util.member "name"
               |> Yojson.Safe.Util.to_string
             in
             let email =
               json
               |> Yojson.Safe.Util.member "email"
               |> Yojson.Safe.Util.to_string
             in
             let%lwt user_result = User.create_user name email in
             match user_result with
             | Ok user_id ->
                 Dream.json
                   (Printf.sprintf {|{"id": "%s", "name": "%s", "email": "%s"}|}
                      user_id name email)
             | Error err ->
                 Lwt.return
                   (Dream.response ~status:`Internal_Server_Error
                      (Printf.sprintf "Error creating user: %s"
                         (Caqti_error.show err))));
       ]
