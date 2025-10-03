WITH total_seats AS (
    SELECT s.aircraft_code, COUNT(*) AS seat_numbers
    FROM seats s
    GROUP BY s.aircraft_code
),
occupied_seats AS (
    SELECT f.flight_id, COUNT(*) AS occupied_seats_numbers
    FROM flights f
    JOIN boarding_passes bp ON f.flight_id = bp.flight_id
    GROUP BY f.flight_id
),
flight_empty AS (
    SELECT f.flight_id,
           t.seat_numbers - o.occupied_seats_numbers AS empty_seats
    FROM flights f
    JOIN total_seats t ON f.aircraft_code = t.aircraft_code
    JOIN occupied_seats o ON f.flight_id = o.flight_id
)
SELECT flight_id, empty_seats
FROM flight_empty
WHERE empty_seats = (SELECT MAX(empty_seats) FROM flight_empty);
