type user = { id : string; name : string; email : string }

let pool =
  Dotenv.export () |> ignore;
  let connection_uri = Uri.of_string (Sys.getenv "POSTGRES_URL") in
  match Caqti_lwt_unix.connect_pool connection_uri with
  | Ok pool -> pool
  | Error err -> failwith (Caqti_error.show err)

let execute query = Caqti_lwt_unix.Pool.use query pool

let create_user name email =
  execute
  @@ [%rapper
       get_one
         {sql|
        INSERT INTO users (name, email)
        VALUES (%string{name}, %string{email})
        RETURNING @string{id}
        |sql}]
       ~name ~email

let get_user_by_id id =
  execute
  @@ [%rapper
       get_one
         {sql|
        SELECT @string{id}, @string{name}, @string{email} 
        FROM users WHERE id = %string{id}
      |sql}
         record_out]
       ~id

let edit_user user =
  execute
  @@ [%rapper
       get_one
         {sql|
        UPDATE users SET name = %string{name}, email = %string{email} WHERE id = %string{id}
        RETURNING @string{id}
      |sql}]
       ~name:user.name ~email:user.email ~id:user.id

let delete_user id =
  execute
  @@ [%rapper
       get_one
         {sql|
        DELETE FROM users WHERE id = %string{id}
        RETURNING @string{id}
      |sql}]
       ~id
