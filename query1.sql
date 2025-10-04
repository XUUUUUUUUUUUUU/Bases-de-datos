WITH first_flight AS(
    SELECT 
        tf.ticket_no, 
        MIN(f.scheduled_departure), 
        f.departure_airport
    FROM ticket_flights tf 
    JOIN flights f ON tf.flight_id = f.flight_id
    GROUP BY tf.ticket_no, f.departure_airport
),

last_flight AS(
    SELECT
        tf.ticket_no, 
        MAX(f.scheduled_departure), 
        f.arrival_airport
    FROM ticket_flights tf 
    JOIN flights f ON tf.flight_id = f.flight_id
    GROUP BY tf.ticket_no, f.arrival_airport
),

round_tickets AS(
    SELECT
        ff.ticket_no,
        ff.departure_airport
    FROM first_flight ff
    JOIN last_flight lf ON ff.ticket_no = lf.ticket_no 
    WHERE ff.departure_airport = lf.arrival_airport
)

SELECT 
    rt.departure_airport, 
    COUNT(DISTINCT t.book_ref) AS Number_of_bookings
FROM round_tickets rt
JOIN tickets t ON t.ticket_no = rt.ticket_no
GROUP BY rt.departure_airport
ORDER BY rt.departure_airport ASC;