SELECT t.book_ref,
       tf.flight_id
FROM   tickets t
       JOIN ticket_flights tf
         ON t.ticket_no = tf.ticket_no
WHERE  NOT EXISTS (SELECT *
                   FROM   boarding_passes bp
                   WHERE  bp.ticket_no = tf.ticket_no
                          AND bp.flight_id = tf.flight_id)
GROUP  BY t.book_ref,
          tf.flight_id
ORDER  BY book_ref,
          flight_id ASC; 