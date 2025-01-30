# OCaml CRUD Todo App

A full-stack todo application built with OCaml (Dream) backend and ReasonReact frontend, featuring JWT authentication and PostgreSQL database.

https://github.com/user-attachments/assets/81fd43af-36fb-4ebf-bfa3-4ffee1a24581

## Features

- JWT Authentication
- Tasks CRUD operations
- RESTful API
- Protected routes
- Type-safe database queries with Rapper

## Tech Stack

### Backend
- OCaml
- Dream (Web framework)
- Caqti (Database interface)
- PostgreSQL
- JWT (JSON Web Tokens)
- Bcrypt (Password hashing)

### Frontend
- Melange (ReasonML)
- TailwindCSS

## API Routes

| Method | Path | Description | Auth Required |
|--------|------|-------------|---------------|
| POST | `/users` | Create a new user | No |
| POST | `/login` | Login a user | No |
| GET | `/verify-token` | Verify JWT token | Yes |
| POST | `/tasks` | Create a new task | Yes |
| GET | `/tasks` | Get all tasks for user | Yes |
| DELETE | `/tasks/:id` | Delete a task | Yes |
| PUT | `/tasks/:id` | Update a task | Yes |

## Getting Started

### Prerequisites

- OCaml
- Node.js
- PostgreSQL
- OPAM

### Environment Variables

Create a `.env` file in the root directory:
```
POSTGRES_URL=postgresql://admin:password@localhost:5432/ocaml_crud
JWT_SECRET=your_key
```