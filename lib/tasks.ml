type t = { id : string; task : string; user_id : string }

let execute = Db.execute

let create_task task user_id =
  execute
  @@ [%rapper
       get_one
         {sql|
        INSERT INTO tasks (task, user_id)
        VALUES (%string{task}, %string{user_id})
        RETURNING @string{id}, @string{task}, @string{user_id}
        |sql}
         record_out]
       ~task ~user_id

let get_tasks user_id =
  execute
  @@ [%rapper
       get_many
         {sql|
        SELECT @string{id}, @string{task}, @string{user_id} 
        FROM tasks WHERE user_id = %string{user_id}
        |sql}
         record_out]
       ~user_id
