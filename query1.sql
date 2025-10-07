WITH flight_times AS (
    SELECT 
        tf.ticket_no,
        MIN(f.scheduled_departure) AS first_dep_time,
        MAX(f.scheduled_departure) AS last_dep_time
    FROM ticket_flights tf
    JOIN flights f ON tf.flight_id = f.flight_id
    GROUP BY tf.ticket_no
),

first_flight AS (
    SELECT 
        tf.ticket_no,
        f.departure_airport
    FROM ticket_flights tf
    JOIN flights f ON tf.flight_id = f.flight_id
    JOIN flight_times ft ON tf.ticket_no = ft.ticket_no AND f.scheduled_departure = ft.first_dep_time
),

last_flight AS (
    SELECT 
        tf.ticket_no,
        f.arrival_airport
    FROM ticket_flights tf
    JOIN flights f ON tf.flight_id = f.flight_id
    JOIN flight_times ft ON tf.ticket_no = ft.ticket_no AND f.scheduled_departure = ft.last_dep_time
),

round_tickets AS (
    SELECT 
        ff.ticket_no,
        ff.departure_airport
    FROM first_flight ff
    JOIN last_flight lf ON ff.ticket_no = lf.ticket_no
    WHERE ff.departure_airport = lf.arrival_airport
)

SELECT 
    rt.departure_airport,
    COUNT(DISTINCT t.book_ref) AS number_of_bookings
FROM round_tickets rt
JOIN tickets t ON rt.ticket_no = t.ticket_no
GROUP BY rt.departure_airport
ORDER BY rt.departure_airport ASC;
