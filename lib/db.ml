let pool =
  lazy
    (Dotenv.export () |> ignore;
     let connection_uri = Uri.of_string (Sys.getenv "POSTGRES_URL") in
     match Caqti_lwt_unix.connect_pool connection_uri with
     | Ok pool -> pool
     | Error err -> failwith (Caqti_error.show err))

let execute query = Caqti_lwt_unix.Pool.use query (Lazy.force pool)
