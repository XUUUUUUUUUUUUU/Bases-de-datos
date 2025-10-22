SELECT bp.ticket_no,bp.flight_id,boarding_no,seat_no
FROM boarding_passes bp
JOIN ticket_flights tf ON tf.ticket_no=bp.ticket_no and tf.flight_id=bp.flight_id
JOIN tickets t ON t.ticket_no=tf.ticket_no
JOIN bookings b ON b.book_ref=t.book_ref
WHERE b.book_ref='00000F'; ---cambia 00000F por x o la variable que sea