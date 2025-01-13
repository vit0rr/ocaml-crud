open Dream

let () =
  run
  @@ router
       [
         get "/" (fun _ -> html "Hello, World");
         get "hello/:name" (fun request ->
             let name = param request "name" in
             html
               (Printf.sprintf
                  {|
                    <html>
                      <body>
                        <h1>Hello, %s!</h1>
                      </body>
                    </html>
                    |}
                  name));
       ]
