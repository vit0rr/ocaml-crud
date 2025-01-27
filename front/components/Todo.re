type todo = {
  id: int,
  text: string,
  isEditing: bool,
};

[@react.component]
let make = (~setToken) => {
  let (todos, setTodos) = React.useState(() => []);
  let (newTodo, setNewTodo) = React.useState(() => "");

  let logout = () => {
    Dom.Storage.localStorage |> Dom.Storage.removeItem("token");
    setToken(_ => None);
    ReasonReactRouter.push("/");
  };

  let addTodo = event => {
    React.Event.Form.preventDefault(event);
    if (String.trim(newTodo) != "") {
      setTodos(prev =>
        [
          {
            id: Js.Date.now() |> int_of_float,
            text: newTodo,
            isEditing: false,
          },
          ...prev,
        ]
      );
      setNewTodo(_ => "");
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

  <div
    className="min-h-screen grid place-items-center bg-background/50 dark:bg-background/80 relative overflow-hidden">
    <div
      className="absolute inset-0 bg-grid-white/10 [mask-image:radial-gradient(ellipse_at_center,transparent_20%,black)] dark:bg-grid-black/10"
    />
    <div
      className="absolute pointer-events-none inset-0 flex items-center justify-center bg-background [mask-image:radial-gradient(ellipse_at_center,transparent_60%,black)]"
    />
    <div
      className="relative w-full max-w-[600px] p-6 space-y-4 rounded-2xl shadow-[0_2px_8px_2px_rgba(0,0,0,0.08)]
       dark:shadow-[0_2px_8px_2px_rgba(0,0,0,0.25)] bg-white dark:bg-gray-950/90 backdrop-blur-[2px] animate-in
       fade-in-0 slide-in-from-bottom-4 duration-1000">
      <div className="flex flex-col space-y-1.5">
        <h1 className="text-2xl font-semibold tracking-tight text-center">
          {React.string("Todo List")}
        </h1>
      </div>
      <form onSubmit=addTodo className="space-y-4">
        <div className="flex gap-2">
          <input
            type_="text"
            value=newTodo
            onChange={e => setNewTodo(React.Event.Form.target(e)##value)}
            className="flex-1 h-10 rounded-md shadow-[0_2px_4px_0px_rgba(0,0,0,0.05)] dark:shadow-[0_2px_4px_0px_rgba(0,0,0,0.15)]
            bg-white dark:bg-gray-950 px-3 py-2 text-sm ring-offset-background file:border-0 file:bg-transparent file:text-sm file:font-medium
            placeholder:text-muted-foreground focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2
            disabled:cursor-not-allowed disabled:opacity-50 hover:shadow-[0_2px_4px_1px_rgba(0,0,0,0.07)] dark:hover:shadow-[0_2px_4px_1px_rgba(0,0,0,0.2)] transition-shadow"
            placeholder="Add a new todo"
          />
          <button
            type_="submit"
            className="inline-flex items-center justify-center rounded-md bg-primary text-primary-foreground hover:bg-primary/90 h-10
             px-4 py-2 text-sm font-medium transition-all focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring
             focus-visible:ring-offset-2 disabled:opacity-50 disabled:pointer-events-none shadow-sm active:scale-[0.98]">
            {React.string("Add")}
          </button>
        </div>
      </form>
      <div className="space-y-2">
        {todos
         |> List.map(todo =>
              <div
                key={string_of_int(todo.id)}
                className="flex items-center gap-2 p-2 rounded-lg hover:bg-gray-50 dark:hover:bg-gray-900/50 transition-colors">
                {todo.isEditing
                   ? <input
                       type_="text"
                       value={todo.text}
                       onChange={e =>
                         handleEditChange(
                           todo.id,
                           React.Event.Form.target(e)##value,
                         )
                       }
                       className="flex-1 h-9 rounded-md shadow-[0_2px_4px_0px_rgba(0,0,0,0.05)] dark:shadow-[0_2px_4px_0px_rgba(0,0,0,0.15)]
                        bg-white dark:bg-gray-950 px-3 py-2 text-sm ring-offset-background file:border-0 file:bg-transparent file:text-sm
                        file:font-medium placeholder:text-muted-foreground focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring
                        focus-visible:ring-offset-2 disabled:cursor-not-allowed disabled:opacity-50 hover:shadow-[0_2px_4px_1px_rgba(0,0,0,0.07)]
                        dark:hover:shadow-[0_2px_4px_1px_rgba(0,0,0,0.2)] transition-shadow"
                     />
                   : <span className="flex-1 px-3 py-1.5">
                       {React.string(todo.text)}
                     </span>}
                <button
                  onClick={_ =>
                    if (todo.isEditing) {
                      updateTodo(todo.id, todo.text);
                    } else {
                      toggleEdit(todo.id);
                    }
                  }
                  className="inline-flex items-center justify-center rounded-md bg-amber-500 text-white hover:bg-amber-600 h-9 px-3 py-2 text-sm
                    font-medium transition-all focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2
                    disabled:opacity-50 disabled:pointer-events-none shadow-sm active:scale-[0.98]">
                  {React.string(todo.isEditing ? "Save" : "Edit")}
                </button>
                <button
                  onClick={_ => deleteTodo(todo.id)}
                  className="inline-flex items-center justify-center rounded-md bg-destructive text-destructive-foreground hover:bg-destructive/90
                    h-9 px-3 py-2 text-sm font-medium transition-all focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring
                    focus-visible:ring-offset-2 disabled:opacity-50 disabled:pointer-events-none shadow-sm active:scale-[0.98]">
                  {React.string("Delete")}
                </button>
              </div>
            )
         |> Array.of_list
         |> React.array}
      </div>
      <div className="pt-4">
        <button
          onClick={_event => logout()}
          className="inline-flex w-full items-center justify-center rounded-md bg-destructive text-destructive-foreground hover:bg-destructive/90
          h-10 px-4 py-2 text-sm font-medium transition-all focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring
          focus-visible:ring-offset-2 disabled:opacity-50 disabled:pointer-events-none shadow-sm active:scale-[0.98]">
          {React.string("Logout")}
        </button>
      </div>
    </div>
  </div>;
};
