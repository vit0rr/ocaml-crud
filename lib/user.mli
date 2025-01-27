(** Represents a user in the system *)
type t = {id: string; email: string}

(** Represents user credentials including password *)
type credentials = {id: string; email: string; password: string}

(** [create_user email password] creates a new user with the given email and password.
    Returns the user's ID if successful, or an error if the operation fails.

    @param email The user's email
    @param password The user's password
    @return A promise containing either the new user's ID or an error
*)
val create_user : string -> string -> ( t, [> Caqti_error.t ] ) result Lwt.t

val generate_token: t -> (Jose.Jwt.t, string) result

val verify_token: string -> (string, string) result


(** [delete_user id] deletes a user by their ID.
    Returns an empty result if successful, or an error if the operation fails.

    @param id The ID of the user to delete
    @return A promise containing either an empty result or an error
*)
val delete_user : string -> ( string, [> Caqti_error.t ] ) result Lwt.t

val login : string -> string -> (t * Jose.Jwt.t, string) result Lwt.t
