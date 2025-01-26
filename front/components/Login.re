[@react.component]
let make = () => {
  let (isLogin, setIsLogin) = React.useState(() => true);
  let (email, setEmail) = React.useState(() => "");
  let (password, setPassword) = React.useState(() => "");
  let (confirmPassword, _) = React.useState(() => "");

  let getValue = (e: React.Event.Form.t) =>
    React.Event.Form.target(e)##value;

  let onSubmit = (e: React.Event.Mouse.t) => {
    React.Event.Mouse.preventDefault(e);
    Js.Console.log(email);
    Js.Console.log(password);
  };

  <div
    className="h-screen flex items-center justify-center bg-gradient-to-br from-gray-50 to-gray-100 dark:from-gray-900 dark:to-gray-800">
    <div
      className="flex flex-col items-center justify-center gap-6 bg-white dark:bg-gray-800 p-8 rounded-xl shadow-lg w-96">
      <h1 className="text-2xl font-bold text-gray-800 dark:text-gray-100">
        {React.string(isLogin ? "Login" : "Register")}
      </h1>
      <form className="w-full space-y-4">
        <div>
          <label
            className="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-1">
            {React.string("Email")}
          </label>
          <input
            type_="email"
            value=email
            onChange={e => setEmail(getValue(e))}
            className="w-full px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:outline-none focus:ring-2 focus:ring-blue-500 dark:bg-gray-700 dark:text-gray-100"
            required=true
          />
        </div>
        <div>
          <label
            className="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-1">
            {React.string("Password")}
          </label>
          <input
            type_="password"
            value=password
            onChange={e => setPassword(getValue(e))}
            className="w-full px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:outline-none focus:ring-2 focus:ring-blue-500 dark:bg-gray-700 dark:text-gray-100"
            required=true
          />
        </div>
        {!isLogin
           ? <div>
               <label
                 className="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-1">
                 {React.string("Confirm Password")}
               </label>
               <input
                 type_="password"
                 value=confirmPassword
                 onChange={e => Js.log(e)}
                 className="w-full px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:outline-none focus:ring-2 focus:ring-blue-500 dark:bg-gray-700 dark:text-gray-100"
                 required=true
               />
             </div>
           : React.null}
        /*
                 For some reason, the "type_" looks not working. It do not trigger the onSubmit function.
                 So, I use onClick by now to trigger the onSubmit function.
         */
        <button
          onClick={evt => onSubmit(evt)}
          type_="submit"
          className="w-full px-4 py-2 bg-blue-500 hover:bg-blue-600 text-white rounded-lg transition-all duration-200">
          {React.string(isLogin ? "Login" : "Register")}
        </button>
      </form>
      <button
        onClick={_evt => setIsLogin(prev => !prev)}
        className="text-sm text-blue-500 hover:text-blue-600 dark:text-blue-400 dark:hover:text-blue-300">
        {React.string(
           isLogin
             ? "Don't have an account? Register"
             : "Already have an account? Login",
         )}
      </button>
    </div>
  </div>;
};
