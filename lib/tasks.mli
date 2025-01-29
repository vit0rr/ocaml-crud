type t = { id : string; task : string; user_id : string }
    
val create_task : string -> string -> (t, [> Caqti_error.t ]) result Lwt.t

val get_tasks : string -> (t list, [> Caqti_error.t ]) result Lwt.t
