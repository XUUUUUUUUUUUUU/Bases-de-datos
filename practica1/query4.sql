WITH occupied_seat
     AS (SELECT tf.flight_id,
                Count(*) AS asiento_reservado
         FROM   ticket_flights tf
         GROUP  BY tf.flight_id),
     seats_by_aircrafts
     AS (SELECT aircraft_code,
                Count(*) AS total_seat
         FROM   seats
         GROUP  BY aircraft_code),
     seats_by_flights
     AS (SELECT f.flight_id,
                s.total_seat AS seats_number
         FROM   flights f
                JOIN seats_by_aircrafts s
                  ON f.aircraft_code = s.aircraft_code),
     empty_seat
     AS (SELECT sf.flight_id,
                sf.seats_number - os.asiento_reservado AS empty_seat_number
         FROM   seats_by_flights sf
                JOIN occupied_seat os
                  ON os.flight_id = sf.flight_id)
SELECT flight_id,
       empty_seat_number
FROM   empty_seat
WHERE  empty_seat_number = (SELECT Max(empty_seat_number)
                            FROM   empty_seat)
ORDER  BY flight_id; 