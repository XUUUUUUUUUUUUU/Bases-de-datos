SELECT b.book_ref,
       f.flight_id
FROM   bookings b
       JOIN tickets t
         ON b.book_ref = t.book_ref
       JOIN ticket_flights tf
         ON t.ticket_no = tf.ticket_no
       JOIN flights f
         ON tf.flight_id = f.flight_id
EXCEPT
SELECT b2.book_ref,
       f2.flight_id
FROM   boarding_passes bp
       JOIN ticket_flights tf2
         ON bp.ticket_no = tf2.ticket_no
       JOIN flights f2
         ON bp.flight_id = f2.flight_id
       JOIN tickets t2
         ON tf2.ticket_no = t2.ticket_no
       JOIN bookings b2
         ON t2.book_ref = b2.book_ref
ORDER  BY book_ref,
          flight_id ASC; 