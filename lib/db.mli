val execute :
  (Caqti_lwt.connection ->
  ('a, ([> Caqti_error.connect ] as 'b)) result Lwt.t) ->
  ('a, 'b) result Lwt.t
