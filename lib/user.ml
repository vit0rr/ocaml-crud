type user = { id : string; name : string; email : string }

let pool =
  lazy
    (Dotenv.export () |> ignore;
     let connection_uri = Uri.of_string (Sys.getenv "POSTGRES_URL") in
     match Caqti_lwt_unix.connect_pool connection_uri with
     | Ok pool -> pool
     | Error err -> failwith (Caqti_error.show err))

let execute query = Caqti_lwt_unix.Pool.use query (Lazy.force pool)

let get_jwt_secret () =
  try Sys.getenv "JWT_SECRET" with Not_found -> "no-env-bro"

let generate_token user =
  let jwt_secret = get_jwt_secret () in
  let key = Jose.Jwk.make_oct jwt_secret in
  let header = Jose.Header.make_header ~typ:"JWT" ~alg:`HS256 key in
  let payload =
    `Assoc
      [
        ("user_id", `String user.id);
        ("email", `String user.email);
        ( "exp",
          `Int
            (Int64.to_int (Int64.add (Unix.time () |> Int64.of_float) 86400L))
        );
      ]
  in
  Jose.Jwt.sign ~header ~payload key
  |> Result.map_error (fun _ -> "JWT signing failed")

let verify_token token =
  let jwt_secret = get_jwt_secret () in
  let key = Jose.Jwk.make_oct jwt_secret in
  match
    match Jose.Jwt.unsafe_of_string token with
    | Ok jwt -> Jose.Jwt.validate ~jwk:key ~now:(Ptime_clock.now ()) jwt
    | Error e -> Error e
  with
  | Ok jwt -> (
      try
        let user_id =
          jwt.Jose.Jwt.payload
          |> Yojson.Safe.Util.member "user_id"
          |> Yojson.Safe.Util.to_string
        in
        Ok user_id
      with _ -> Error "Invalid token payload")
  | Error _ -> Error "Invalid token"

let create_user name email password =
  let hashed_password = Bcrypt.hash password |> Bcrypt.string_of_hash in
  execute
  @@ [%rapper
       get_one
         {sql|
        INSERT INTO users (name, email, password)
        VALUES (%string{name}, %string{email}, %string{password})
        RETURNING @string{id}
        |sql}]
       ~name ~email ~password:hashed_password

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
