open Hooks.UseValidateForm;

[@react.component]
let make = (~setToken) => {
  let (
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
    getValue,
    onSubmit,
  ) =
    useLoginForm(~setToken);

  <div
    className="h-screen flex items-center justify-center bg-gradient-to-br from-gray-50 to-gray-100 dark:from-gray-900 dark:to-gray-800">
    <div
      className="flex flex-col items-center justify-center gap-6 bg-white dark:bg-gray-800 p-8 rounded-xl shadow-lg w-96">
      <h1 className="text-2xl font-bold text-gray-800 dark:text-gray-100">
        {React.string(isLogin ? "Login" : "Register")}
      </h1>
      {switch (error) {
       | Some(msg) =>
         <div
           className="w-full p-3 bg-red-100 border border-red-400 text-red-700 rounded">
           {React.string(msg)}
         </div>
       | None => React.null
       }}
      <form className="w-full space-y-4" onSubmit>
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
                 onChange={e => setConfirmPassword(getValue(e))}
                 className="w-full px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:outline-none focus:ring-2 focus:ring-blue-500 dark:bg-gray-700 dark:text-gray-100"
                 required=true
               />
             </div>
           : React.null}
        <button
          type_="submit"
          disabled=isLoading
          className="w-full px-4 py-2 bg-blue-500 hover:bg-blue-600 text-white rounded-lg transition-all duration-200 disabled:opacity-50 disabled:cursor-not-allowed">
          {React.string(
             isLoading ? "Loading..." : isLogin ? "Login" : "Register",
           )}
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
