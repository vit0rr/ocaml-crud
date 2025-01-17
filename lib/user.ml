type user = { id : string; name : string; email : string }

let pool =
  Dotenv.export () |> ignore;
  let connection_uri = Uri.of_string (Sys.getenv "POSTGRES_URL") in
  match Caqti_lwt_unix.connect_pool connection_uri with
  | Ok pool -> pool
  | Error err -> failwith (Caqti_error.show err)

let execute query = Caqti_lwt_unix.Pool.use query pool

let create_user_query =
  [%rapper
    get_one
      {sql|
        INSERT INTO users (name, email)
        VALUES (%string{name}, %string{email})
        RETURNING @string{id}
        |sql}]

let get_user_by_id_query =
  let open Caqti_request.Infix in
  (Caqti_type.string ->! Caqti_type.(t3 string string string))
    {| SELECT id, name, email FROM users WHERE id = ? |}

let edit_user_query =
  let open Caqti_request.Infix in
  (Caqti_type.(t3 string string string) ->! Caqti_type.string)
    {| UPDATE users SET name = ?, email = ? WHERE id = ? RETURNING id |}

let delete_user_query =
  let open Caqti_request.Infix in
  (Caqti_type.string ->. Caqti_type.unit) {| DELETE FROM users WHERE id = ? |}

let create_user name email = execute @@ create_user_query ~name ~email

let get_user_by_id id : (user, [> Caqti_error.t ]) result Lwt.t =
  Caqti_lwt_unix.Pool.use
    (fun (module Db : Caqti_lwt.CONNECTION) ->
      let%lwt result = Db.find get_user_by_id_query id in
      match result with
      | Ok (id, name, email) -> Lwt.return_ok { id; name; email }
      | Error _ as err -> Lwt.return err)
    pool

let edit_user user : (string, [> Caqti_error.t ]) result Lwt.t =
  Caqti_lwt_unix.Pool.use
    (fun (module Db : Caqti_lwt.CONNECTION) ->
      Db.find edit_user_query (user.name, user.email, user.id))
    pool

let delete_user id : (unit, [> Caqti_error.t ]) result Lwt.t =
  Caqti_lwt_unix.Pool.use
    (fun (module Db : Caqti_lwt.CONNECTION) -> Db.exec delete_user_query id)
    pool
