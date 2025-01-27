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
    className="min-h-screen grid place-items-center bg-background/50 dark:bg-background/80 relative overflow-hidden">
    <div
      className="absolute inset-0 bg-grid-white/10 [mask-image:radial-gradient(ellipse_at_center,transparent_20%,black)] dark:bg-grid-black/10"
    />
    <div
      className="absolute pointer-events-none inset-0 flex items-center justify-center bg-background [mask-image:radial-gradient(ellipse_at_center,transparent_60%,black)]"
    />
    <div
      className="relative w-full max-w-[400px] p-6 space-y-4 rounded-2xl shadow-[0_2px_8px_2px_rgba(0,0,0,0.08)] dark:shadow-[0_2px_8px_2px_rgba(0,0,0,0.25)] bg-white dark:bg-gray-950/90 backdrop-blur-[2px] animate-in fade-in-0 slide-in-from-bottom-4 duration-1000">
      <div className="flex flex-col space-y-1.5">
        <h1 className="text-2xl font-semibold tracking-tight text-center">
          {React.string(isLogin ? "Welcome back" : "Create an account")}
        </h1>
        {!isLogin
           ? <p className="text-sm text-center text-muted-foreground">
               {React.string("Enter your email below to create your account")}
             </p>
           : React.null}
      </div>
      {switch (error) {
       | Some(msg) =>
         <div
           className="p-3 text-sm bg-destructive/15 text-destructive rounded-md border border-destructive/20">
           {React.string(msg)}
         </div>
       | None => React.null
       }}
      <form className="space-y-4" onSubmit>
        <div className="space-y-2">
          <label
            className="text-sm font-medium leading-none text-muted-foreground">
            {React.string("Email")}
          </label>
          <input
            type_="email"
            value=email
            onChange={e => setEmail(getValue(e))}
            placeholder="m@example.com"
            className="flex h-10 w-full rounded-md shadow-[0_2px_4px_0px_rgba(0,0,0,0.05)] dark:shadow-[0_2px_4px_0px_rgba(0,0,0,0.15)] bg-white dark:bg-gray-950 px-3 py-2 text-sm ring-offset-background file:border-0 file:bg-transparent file:text-sm file:font-medium placeholder:text-muted-foreground focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2 disabled:cursor-not-allowed disabled:opacity-50 hover:shadow-[0_2px_4px_1px_rgba(0,0,0,0.07)] dark:hover:shadow-[0_2px_4px_1px_rgba(0,0,0,0.2)] transition-shadow"
            required=true
          />
        </div>
        <div className="space-y-2">
          <label
            className="text-sm font-medium leading-none text-muted-foreground">
            {React.string("Password")}
          </label>
          <input
            type_="password"
            value=password
            onChange={e => setPassword(getValue(e))}
            className="flex h-10 w-full rounded-md shadow-[0_2px_4px_0px_rgba(0,0,0,0.05)] dark:shadow-[0_2px_4px_0px_rgba(0,0,0,0.15)] bg-white dark:bg-gray-950 px-3 py-2 text-sm ring-offset-background file:border-0 file:bg-transparent file:text-sm file:font-medium placeholder:text-muted-foreground focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2 disabled:cursor-not-allowed disabled:opacity-50 hover:shadow-[0_2px_4px_1px_rgba(0,0,0,0.07)] dark:hover:shadow-[0_2px_4px_1px_rgba(0,0,0,0.2)] transition-shadow"
            required=true
          />
        </div>
        {!isLogin
           ? <div className="space-y-2">
               <label
                 className="text-sm font-medium leading-none text-muted-foreground">
                 {React.string("Confirm Password")}
               </label>
               <input
                 type_="password"
                 value=confirmPassword
                 onChange={e => setConfirmPassword(getValue(e))}
                 className="flex h-10 w-full rounded-md shadow-[0_2px_4px_0px_rgba(0,0,0,0.05)] dark:shadow-[0_2px_4px_0px_rgba(0,0,0,0.15)] bg-white dark:bg-gray-950 px-3 py-2 text-sm ring-offset-background file:border-0 file:bg-transparent file:text-sm file:font-medium placeholder:text-muted-foreground focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2 disabled:cursor-not-allowed disabled:opacity-50 hover:shadow-[0_2px_4px_1px_rgba(0,0,0,0.07)] dark:hover:shadow-[0_2px_4px_1px_rgba(0,0,0,0.2)] transition-shadow"
                 required=true
               />
             </div>
           : React.null}
        <button
          type_="submit"
          disabled=isLoading
          className="inline-flex w-full items-center justify-center rounded-md bg-black dark:bg-white text-white dark:text-black hover:bg-black/90 dark:hover:bg-white/90 h-10 px-4 py-2 text-sm font-medium transition-all focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2 disabled:opacity-50 disabled:pointer-events-none shadow-sm active:scale-[0.98]">
          {React.string(
             isLoading
               ? "Loading..." : isLogin ? " Sign In" : "Create account",
           )}
        </button>
      </form>
      <button
        onClick={_evt => setIsLogin(prev => !prev)}
        className="text-sm text-center text-muted-foreground hover:text-primary underline-offset-4 hover:underline mx-auto block transition-colors duration-200">
        {React.string(
           isLogin
             ? "Don't have an account? Sign Up"
             : "Already have an account? Sign In",
         )}
      </button>
    </div>
  </div>;
};
