SELECT b.book_ref,
       b.total_amount,
       Sum(tf.amount)
FROM   bookings b
       JOIN tickets t
         ON t.book_ref = b.book_ref
       JOIN ticket_flights tf
         ON tf.ticket_no = t.ticket_no
GROUP  BY b.book_ref,
          b.total_amount
ORDER  BY b.book_ref ASC; 