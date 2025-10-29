WITH booking_ticket AS( /* Nos devuelve todos los tickes de una book_ref*/
    SELECT b.book_ref,tf.ticket_no,tf.flight_id
    FROM bookings b
    JOIN tickets t ON t.book_ref=b.book_ref
    JOIN ticket_flights tf ON tf.ticket_no=t.ticket_no
    WHERE (tf.ticket_no,tf.flight_id) NOT IN (
        SELECT bp.ticket_no,bp.flight_id
        FROM boarding_passes bp
    )
), flight_available_seat_no AS ( /* Nos devuelve asientos libres de cada vuelo que todavia no esta asignado*/
    SELECT bt.ticket_no,bt.flight_id,f.aircraft_code,s.seat_no
    FROM booking_ticket bt
    JOIN flights f ON f.flight_id=bt.flight_id
    JOIN seats s ON s.aircraft_code=f.aircraft_code
    WHERE (f.aircraft_code,s.seat_no) IN (
        SELECT s2.aircraft_code,s2.seat_no
        FROM seats s2
        WHERE NOT EXISTS (
            SELECT 1
            FROM boarding_passes bp 
            JOIN flights f2 ON f2.flight_id=bp.flight_id
            WHERE f2.aircraft_code=s2.aircraft_code AND
                s2.seat_no=bp.seat_no
        )
    )
    ORDER BY f.aircraft_code, s.seat_no ASC
)
