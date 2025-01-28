type t = { id : string; email : string }
type credentials = { id : string; email : string; password : string }

let execute = Db.execute

let get_jwt_secret () =
  try Sys.getenv "JWT_SECRET" with Not_found -> "no-env-bro"

let generate_token (user : t) =
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

let create_user email password =
  let hashed_password = Bcrypt.hash password |> Bcrypt.string_of_hash in
  execute
  @@ [%rapper
       get_one
         {sql|
        INSERT INTO users (email, password)
        VALUES (%string{email}, %string{password})
        RETURNING @string{id}, @string{email}
        |sql}
         record_out]
       ~email ~password:hashed_password

let delete_user id =
  execute
  @@ [%rapper
       get_one
         {sql|
        DELETE FROM users WHERE id = %string{id}
        RETURNING @string{id}
      |sql}]
       ~id

let get_user_by_email email =
  execute
  @@ [%rapper
       get_one
         {sql|
        SELECT @string{id}, @string{email}, @string{password}
        FROM users WHERE email = %string{email}
      |sql}
         record_out]
       ~email

let login email password =
  match%lwt get_user_by_email email with
  | Error _ -> Lwt.return (Error "Invalid email or password")
  | Ok user_with_password ->
      if
        Bcrypt.verify password
          (Bcrypt.hash_of_string user_with_password.password)
      then
        let user =
          { id = user_with_password.id; email = user_with_password.email }
        in
        match generate_token user with
        | Ok token -> Lwt.return (Ok (user, token))
        | Error err -> Lwt.return (Error err)
      else Lwt.return (Error "Invalid email or password")
