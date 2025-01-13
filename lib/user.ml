type user = { id : string; name : string; email : string }

let pool =
  Dotenv.export () |> ignore;
  let connection_uri = Uri.of_string (Sys.getenv "POSTGRES_URL") in
  let config =
    Caqti_connect_config.default
    |> Caqti_connect_config.set Caqti_connect_config.tweaks_version (1, 0)
  in
  match Caqti_lwt_unix.connect_pool ~config connection_uri with
  | Ok pool -> pool
  | Error err -> failwith (Caqti_error.show err)

let create_user_query =
  let open Caqti_request.Infix in
  (Caqti_type.(t2 string string) ->! Caqti_type.string)
    {| INSERT INTO users (name, email) VALUES (?, ?) RETURNING id|}

let create_user name email : (string, [> Caqti_error.t ]) result Lwt.t =
  Caqti_lwt_unix.Pool.use
    (fun (module Db : Caqti_lwt.CONNECTION) ->
      Db.find create_user_query (name, email))
    pool
