open Js.Promise;
open Fetch;
open Belt;

type user = {
  id: string,
  name: string,
  email: string,
};

[@react.component]
let make = () => {
  let fetchUserName = () => {
    // TODO: I can probably improve this code
    let url = "http://localhost:8080/users/c8d31481-fa56-4513-a5a4-14471646bb3b";
    let response =
      fetch(url)
      |> then_(Fetch.Response.json)
      |> then_(json =>
           switch (Js.Json.decodeObject(json)) {
           | Some(obj) =>
             resolve({
               id:
                 Js.Dict.get(obj, "id")
                 |> Belt.Option.flatMap(_, Js.Json.decodeString)
                 |> Belt.Option.getWithDefault(_, ""),
               name:
                 Js.Dict.get(obj, "name")
                 |> Belt.Option.flatMap(_, Js.Json.decodeString)
                 |> Belt.Option.getWithDefault(_, ""),
               email:
                 Js.Dict.get(obj, "email")
                 |> Belt.Option.flatMap(_, Js.Json.decodeString)
                 |> Belt.Option.getWithDefault(_, ""),
             })
           | None =>
             resolve({
               id: "",
               name: "",
               email: "",
             })
           }
         )
      |> catch(error => {
           Js.log(error);
           resolve({
             id: "",
             name: "",
             email: "",
           });
         });
    response;
  };

  let (counter, setCounter) = React.useState(() => 0);
  let (user, setUser) =
    React.useState(() =>
      {
        id: "",
        name: "",
        email: "",
      }
    );

  <div
    className="h-screen flex items-center justify-center bg-gradient-to-br from-gray-50 to-gray-100 dark:from-gray-900 dark:to-gray-800">
    <div
      className="flex flex-col items-center justify-center gap-6 bg-white dark:bg-gray-800 p-8 rounded-xl shadow-lg">
      <div className="flex flex-row items-center justify-center gap-6">
        <button
          className="w-12 h-12 flex items-center justify-center bg-gray-100 hover:bg-gray-200 dark:bg-gray-700 dark:hover:bg-gray-600 text-gray-600 dark:text-gray-200 text-xl font-medium rounded-lg transition-all duration-200 focus:outline-none focus:ring-2 focus:ring-gray-300 dark:focus:ring-gray-500"
          onClick={_evt => setCounter(v => v - 1)}>
          {React.string("-")}
        </button>
        <span
          className="text-4xl font-bold text-gray-800 dark:text-gray-100 w-20 text-center tabular-nums">
          {React.string(Int.toString(counter))}
        </span>
        <button
          className="w-12 h-12 flex items-center justify-center bg-gray-100 hover:bg-gray-200 dark:bg-gray-700 dark:hover:bg-gray-600 text-gray-600 dark:text-gray-200 text-xl font-medium rounded-lg transition-all duration-200 focus:outline-none focus:ring-2 focus:ring-gray-300 dark:focus:ring-gray-500"
          onClick={_evt => setCounter(v => v + 1)}>
          {React.string("+")}
        </button>
      </div>
      <button
        className="px-4 py-2 bg-blue-500 hover:bg-blue-600 text-white rounded-lg"
        onClick={_evt => {
          fetchUserName()
          |> then_(user => {
               setUser(_ => user);
               resolve();
             })
          |> ignore
        }}>
        {React.string("Fetch Username")}
      </button>
      <div className="text-gray-800 dark:text-gray-100">
        {React.string("Username: " ++ user.name)}
      </div>
    </div>
  </div>;
};
