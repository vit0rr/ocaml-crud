type todo = {
  id: string,
  text: string,
  isEditing: bool,
};

let useTodo = (~setToken, ~token) => {
  let (todos, setTodos) = React.useState(() => []);
  let (newTodo, setNewTodo) = React.useState(() => "");
  let (error, setError) = React.useState(() => None);
  let (isLoading, setIsLoading) = React.useState(() => false);

  let logout = () => {
    Dom.Storage.localStorage |> Dom.Storage.removeItem("token");
    setToken(_ => None);
    ReasonReactRouter.push("/");
  };

  let addTodo = event => {
    React.Event.Form.preventDefault(event);
    let trimmedTodo = String.trim(newTodo);

    if (trimmedTodo != "") {
      setIsLoading(_ => true);
      setError(_ => None);

      let payload = Js.Dict.empty();
      Js.Dict.set(payload, "task", Js.Json.string(trimmedTodo));

      Fetch.fetchWithInit(
        "http://localhost:8080/tasks",
        Fetch.RequestInit.make(
          ~method_=Post,
          ~body=
            payload
            |> Js.Json.object_
            |> Js.Json.stringify
            |> Fetch.BodyInit.make,
          ~headers=
            Fetch.HeadersInit.make({
              "Content-Type": "application/json",
              "Authorization": "Bearer " ++ token,
            }),
          (),
        ),
      )
      |> Js.Promise.then_(Fetch.Response.json)
      |> Js.Promise.then_(json => {
           setIsLoading(_ => false);
           switch (Js.Json.decodeObject(json)) {
           | Some(obj) =>
             switch (
               Js.Dict.get(obj, "id"),
               Js.Dict.get(obj, "task"),
               Js.Dict.get(obj, "error"),
             ) {
             | (Some(idJson), Some(taskJson), None) =>
               switch (
                 Js.Json.decodeString(idJson),
                 Js.Json.decodeString(taskJson),
               ) {
               | (Some(id), Some(task)) =>
                 setTodos(prev =>
                   [
                     {
                       id,
                       text: task,
                       isEditing: false,
                     },
                     ...prev,
                   ]
                 );
                 setNewTodo(_ => "");
               | _ => setError(_ => Some("Invalid response format"))
               }
             | (_, _, Some(errorJson)) =>
               switch (Js.Json.decodeString(errorJson)) {
               | Some(errorMsg) => setError(_ => Some(errorMsg))
               | None => setError(_ => Some("An unexpected error occurred"))
               }
             | _ => setError(_ => Some("Invalid response format"))
             }
           | None => setError(_ => Some("Invalid response format"))
           };
           Js.Promise.resolve();
         })
      |> Js.Promise.catch(err => {
           setIsLoading(_ => false);
           setError(_ => Some("Failed to create todo. Please try again."));
           Js.Console.error(err);
           Js.Promise.resolve();
         })
      |> ignore;
    };
  };

  let deleteTodo = id => {
    setTodos(prev => prev |> List.filter(todo => todo.id != id));
  };

  let toggleEdit = id => {
    setTodos(prev =>
      prev
      |> List.map(todo =>
           todo.id == id
             ? {
               ...todo,
               isEditing: !todo.isEditing,
             }
             : todo
         )
    );
  };

  let updateTodo = (id, newText) => {
    setTodos(prev =>
      prev
      |> List.map(todo =>
           todo.id == id
             ? {
               ...todo,
               text: newText,
               isEditing: false,
             }
             : todo
         )
    );
  };

  let handleEditChange = (id, newText) => {
    setTodos(prev =>
      prev
      |> List.map(todo =>
           todo.id == id
             ? {
               ...todo,
               text: newText,
             }
             : todo
         )
    );
  };

  React.useEffect0(() => {
    Fetch.fetchWithInit(
      "http://localhost:8080/tasks",
      Fetch.RequestInit.make(
        ~method_=Get,
        ~headers=
          Fetch.HeadersInit.make({"Authorization": "Bearer " ++ token}),
        (),
      ),
    )
    |> Js.Promise.then_(Fetch.Response.json)
    |> Js.Promise.then_(json => {
         switch (Js.Json.decodeArray(json)) {
         | Some(array) =>
           let todos =
             array
             |> Array.map(item =>
                  switch (Js.Json.decodeObject(item)) {
                  | Some(obj) =>
                    switch (
                      Js.Dict.get(obj, "id"),
                      Js.Dict.get(obj, "task"),
                    ) {
                    | (Some(idJson), Some(taskJson)) =>
                      switch (
                        Js.Json.decodeString(idJson),
                        Js.Json.decodeString(taskJson),
                      ) {
                      | (Some(id), Some(task)) =>
                        Some({
                          id,
                          text: task,
                          isEditing: false,
                        })
                      | _ => None
                      }
                    | _ => None
                    }
                  | None => None
                  }
                )
             |> Array.to_list
             |> List.filter(item =>
                  switch (item) {
                  | Some(_) => true
                  | None => false
                  }
                )
             |> List.map(item =>
                  switch (item) {
                  | Some(todo) => todo
                  | None => assert(false)
                  }
                );
           setTodos(_ => todos);
         | None => setError(_ => Some("Failed to fetch todos"))
         };
         Js.Promise.resolve();
       })
    |> Js.Promise.catch(err => {
         setError(_ => Some("Failed to fetch todos"));
         Js.Console.error(err);
         Js.Promise.resolve();
       })
    |> ignore;

    None;
  });

  (
    // States
    todos,
    newTodo,
    setNewTodo,
    error,
    isLoading,
    // Functions
    logout,
    addTodo,
    deleteTodo,
    toggleEdit,
    updateTodo,
    handleEditChange,
  );
};
