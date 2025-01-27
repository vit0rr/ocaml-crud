open Components;

module App = {
  [@react.component]
  let make = () => {
    let url = ReasonReactRouter.useUrl();
    let (token, setToken) = React.useState(() => None);
    let (isLoading, setIsLoading) = React.useState(() => true);

    React.useEffect1(
      () => {
        switch (Dom.Storage.localStorage |> Dom.Storage.getItem("token")) {
        | Some(savedToken) =>
          Fetch.fetchWithInit(
            "http://localhost:8080/verify-token",
            Fetch.RequestInit.make(
              ~method_=Get,
              ~headers=
                Fetch.HeadersInit.make({
                  "Authorization": "Bearer " ++ savedToken,
                }),
              (),
            ),
          )
          |> Js.Promise.then_(response => {
               if (Fetch.Response.status(response) === 200) {
                 setToken(_ => Some(savedToken));
                 setIsLoading(_ => false);
                 if (url.path == []) {
                   ReasonReactRouter.push("/todo");
                 };
               } else {
                 Dom.Storage.localStorage |> Dom.Storage.removeItem("token");
                 setIsLoading(_ => false);
               };
               Js.Promise.resolve();
             })
          |> ignore
        | None => setIsLoading(_ => false)
        };
        None;
      },
      [|url.path|],
    );

    if (isLoading) {
      <div> {React.string("Loading...")} </div>;
    } else {
      switch (url.path) {
      | [] =>
        switch (token) {
        | Some(_) =>
          ReasonReactRouter.push("/todo");
          <div> {React.string("Redirecting...")} </div>;
        | None => <Login setToken />
        }
      | ["todo"] =>
        switch (token) {
        | Some(_) => <Todo setToken />
        | None =>
          ReasonReactRouter.push("/");
          <div> {React.string("Redirecting...")} </div>;
        }
      | _ => <div> {React.string("Not found")} </div>
      };
    };
  };
};

let node = ReactDOM.querySelector("#root");
switch (node) {
| None =>
  Js.Console.error("Failed to start React: couldn't find the #root element")
| Some(root) =>
  let root = ReactDOM.Client.createRoot(root);
  ReactDOM.Client.render(root, <App />);
};
