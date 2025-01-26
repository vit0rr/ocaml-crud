open Alcotest

let () = Dotenv.export () |> ignore

let test_user =
  {
    Ocaml_crud.User.id = "123";
    name = "Vitor Souza";
    email = "vitor@example.com";
  }

let test_generate_and_verify_token () =
  match Ocaml_crud.User.generate_token test_user with
  | Ok token -> (
      match Ocaml_crud.User.verify_token (Jose.Jwt.to_string token) with
      | Ok user_id ->
          Printf.printf "\nGenerated JWT: %s\n" (Jose.Jwt.to_string token);
          check string "user_id matches" test_user.id user_id
      | Error msg -> fail ("Token verification failed: " ^ msg))
  | Error msg -> fail ("Token generation failed: " ^ msg)

let test_verify_invalid_token () =
  let invalid_token =
    "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIn0.wrong_signature"
  in
  match Ocaml_crud.User.verify_token invalid_token with
  | Ok _ -> fail "Expected error for invalid token"
  | Error msg -> check string "error message" "Invalid token" msg

let () =
  run "User"
    [
      ( "jwt",
        [
          test_case "generate and verify token" `Quick
            test_generate_and_verify_token;
          test_case "verify invalid token" `Quick test_verify_invalid_token;
        ] );
    ]
