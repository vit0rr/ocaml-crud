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
