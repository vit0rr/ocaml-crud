let useLoginForm = (~setToken) => {
  // TODO: it seems too much. Maybe split into multiple hooks
  let (email, setEmail) = React.useState(() => "");
  let (password, setPassword) = React.useState(() => "");
  let (confirmPassword, setConfirmPassword) = React.useState(() => "");
  let (isLogin, setIsLogin) = React.useState(() => true);
  let (error, setError) = React.useState(() => None);
  let (isLoading, setIsLoading) = React.useState(() => false);

  let validateForm = () =>
    switch (email, password, isLogin, confirmPassword) {
    | ("", _, _, _) => Error("Email is required")
    | (_, "", _, _) => Error("Password is required")
    | (_, _, false, conf) when password != conf =>
      Error("Passwords do not match")
    | _ => Ok()
    };

  let getValue = (e: React.Event.Form.t) =>
    React.Event.Form.target(e)##value;

  let useAuthRequest = () => {
    let payload = Js.Dict.empty();
    Js.Dict.set(payload, "email", Js.Json.string(email));
    Js.Dict.set(payload, "password", Js.Json.string(password));

    let endpoint = isLogin ? "login" : "users";

    setIsLoading(_ => true);
    setError(_ => None);

    Fetch.fetchWithInit(
      "http://localhost:8080/" ++ endpoint,
      Fetch.RequestInit.make(
        ~method_=Post,
        ~body=
          payload
          |> Js.Json.object_
          |> Js.Json.stringify
          |> Fetch.BodyInit.make,
        ~headers=Fetch.HeadersInit.make({"Content-Type": "application/json"}),
        (),
      ),
    )
    |> Js.Promise.then_(Fetch.Response.json)
    |> Js.Promise.then_(json => {
         setIsLoading(_ => false);
         switch (Js.Json.decodeObject(json)) {
         | Some(obj) =>
           switch (Js.Dict.get(obj, "error")) {
           | Some(errorJson) =>
             switch (Js.Json.decodeString(errorJson)) {
             | Some(errorMsg) => setError(_ => Some(errorMsg))
             | None => setError(_ => Some("An unexpected error occurred"))
             }
           | None =>
             if (isLogin) {
               switch (Js.Dict.get(obj, "token")) {
               | Some(token) =>
                 let tokenStr = Js.Json.decodeString(token);
                 switch (tokenStr) {
                 | Some(t) =>
                   Dom.Storage.localStorage |> Dom.Storage.setItem("token", t);
                   setToken(_ => Some(t));
                   ReasonReactRouter.push("/todo");
                 | None => setError(_ => Some("Invalid token received"))
                 };
               | None => setError(_ => Some("No token received"))
               };
             } else {
               setIsLogin(_ => true);
             }
           }
         | None => setError(_ => Some("Invalid response format"))
         };
         Js.Promise.resolve();
       })
    |> Js.Promise.catch(err => {
         setIsLoading(_ => false);
         setError(_ => Some("Authentication failed. Please try again."));
         Js.Console.error(err);
         Js.Promise.resolve();
       })
    |> ignore;
  };

  let onSubmit = (e: React.Event.Form.t) => {
    React.Event.Form.preventDefault(e);

    switch (validateForm()) {
    | Ok () => useAuthRequest()
    | Error(msg) => setError(_ => Some(msg))
    };
  };

  (
    // States
    email,
    password,
    confirmPassword,
    isLogin,
    setEmail,
    setPassword,
    setConfirmPassword,
    setIsLogin,
    error,
    isLoading,
    // Functions
    getValue,
    onSubmit,
  );
};
