(** Represents a user in the system *)
type user = {id: string; name: string; email: string}

(** [create_user name email] creates a new user with the given name and email.
    Returns the user's ID if successful, or an error if the operation fails.

    @param name The user's name
    @param email The user's email
    @return A promise containing either the new user's ID or an error
*)
val create_user : string -> string -> ( string, [> Caqti_error.t ] ) result Lwt.t

(** [get_user_by_id id] retrieves a user by their ID.
    Returns the user if found, or an error if the operation fails.

    @param id The ID of the user to retrieve
    @return A promise containing either the user or an error
*)
val get_user_by_id : string -> ( user, [> Caqti_error.t ] ) result Lwt.t

(** [edit_user user] edits a user with the given user object.
    Returns the user's ID if successful, or an error if the operation fails.

    @param user The user object to edit
    @return A promise containing either the user's ID or an error
*)
val edit_user : user -> ( string, [> Caqti_error.t ] ) result Lwt.t

(** [delete_user id] deletes a user by their ID.
    Returns an empty result if successful, or an error if the operation fails.

    @param id The ID of the user to delete
    @return A promise containing either an empty result or an error
*)
val delete_user : string -> ( string, [> Caqti_error.t ] ) result Lwt.t

