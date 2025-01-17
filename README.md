# OCaml CRUD

This is a simple CRUD application built with OCaml and PostgreSQL.

- GET `/users/:id` - Get a user by their ID
    
    Response:
    ```JSON
    {
        "id": "c8d31481-fa56-4513-a5a4-14471646bb3b",
        "name": "Vitor S. Almeida",
        "email": "vitor@example.com"
    }
    ```

- POST `/users` - Create a new user
    
    Body:
    ```JSON
    {
        "name": "Vitor",
        "email": "vitor-email@example.com"
    }
    ```
    Response:
    ```JSON
    {
        "id": "3adb282e-80e6-442d-93b6-9b1183e08274",
        "name": "Vitor",
        "email": "vitor-email@example.com"
    }
    ```

- PUT `/users/:id` - Update a user
    
    Body:
    ```JSON
    {
        "name": "Vitor New Name",
        "email": "vitor-new-email@example.com"
    }
    ```
    Response:
    ```JSON
    {
        "id": "3adb282e-80e6-442d-93b6-9b1183e08274",
        "name": "Vitor New Name",
        "email": "vitor-new-email@example.com"
    }
    ```

- DELETE `/users/:id` - Delete a user

    Response:
    ```JSON
    {
        "id": "3adb282e-80e6-442d-93b6-9b1183e08274",
        "message": "User deleted"
    }
    ```

# How to run

```bash
docker compose up -d
dune build
dune exec ocaml_crud -w
```
You may have issues with the ppx_rapper_lwt. If you do, try to pin the dev. version:
```bash
opam pin ppx_rapper git+https://github.com/roddyyaga/ppx_rapper.git
```

And I'm open to issues! Feel free to open an issue if you have any questions or suggestions.