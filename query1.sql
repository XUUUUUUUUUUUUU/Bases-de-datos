SELECT a.airport_code AS bookings
FROM bookings b
JOIN tickets t ON b.book_ref = t.book_ref
JOIN airport_data a ON a.airport_code IN (
    SELECT a2.airport_code
    FROM ticket_flights tf
    JOIN tickets t2 ON t2.ticket_no = tf.ticket_no
    JOIN flights f ON tf.flight_id = f.flight_id
    JOIN airport_data a2 ON f.departure_airport = a2.airport_code
                         OR f.arrival_airport = a2.airport_code
    WHERE f.departure_airport = f.arrival_airport
)
ORDER BY a.airport_code ASC;
