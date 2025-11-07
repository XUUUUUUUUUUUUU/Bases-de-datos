SELECT DISTINCT b.book_ref
FROM bookings b
JOIN tickets t ON t.book_ref = b.book_ref
JOIN ticket_flights tf ON tf.ticket_no = t.ticket_no
WHERE NOT EXISTS (
    SELECT 1
    FROM boarding_passes bp
    WHERE bp.ticket_no = tf.ticket_no
      AND bp.flight_id = tf.flight_id
)
ORDER BY b.book_ref;
