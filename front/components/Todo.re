[@react.component]
let make = (~setToken) => {
  let logout = () => {
    Dom.Storage.localStorage |> Dom.Storage.removeItem("token");
    setToken(_ => None);
    ReasonReactRouter.push("/");
  };

  <div
    className="min-h-screen bg-gray-100 flex flex-col items-center justify-center p-4">
    <div className="bg-white rounded-lg shadow-md p-8 max-w-md w-full">
      <h1 className="text-2xl font-bold text-gray-800 mb-6 text-center">
        {React.string("Protected Page")}
      </h1>
      <button
        onClick={_event => logout()}
        className="w-full bg-red-500 hover:bg-red-600 text-white font-semibold py-2 px-4 rounded-md transition duration-200 ease-in-out">
        {React.string("Logout")}
      </button>
    </div>
  </div>;
};
