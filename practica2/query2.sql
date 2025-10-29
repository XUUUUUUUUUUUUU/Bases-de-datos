SELECT b.book_ref,t.passenger_name,f.flight_id,t.ticket_no, MAX(bp.seat_no)+1
FROM booking b
JOIN tickets t ON t.book_ref=b.book_ref
JOIN ticket_flights tf ON tf.ticket_no=t.ticket_no
JOIN boarding_passes bp ON bp.flight_id=tf.flight_id
JOIN flights f ON f.flight_id=tf.flight_id
WHERE 
GROUP BY b.book_ref, t.passenger_name,f.flight_id,t.ticket_no;