WITH direct_flights AS (
                               SELECT f.flight_id AS first_flight, 
                                      f.scheduled_departure, f.scheduled_arrival, 0 AS connections, 
                                      ( 
                                          SELECT COUNT(*) 
                                          FROM seats s 
                                          WHERE s.aircraft_code = f.aircraft_code 
                                            AND s.seat_no NOT IN ( 
                                                SELECT bp.seat_no 
                                                FROM boarding_passes bp 
                                                JOIN ticket_flights tf ON bp.ticket_no = tf.ticket_no 
                                                WHERE tf.flight_id = f.flight_id 
                                            ) 
                                      ) AS free_seats, 
                                      (f.scheduled_arrival - f.scheduled_departure) AS total_duration,
                              			  f.flight_id AS first_flight_id,
                              			  f.flight_id AS second_flight_id,
                              			  f.aircraft_code AS first_aircraft_code,
                              			  f.aircraft_code AS second_aircraft_code,
                              			  f.scheduled_departure AS first_departure_time,
                              			  f.scheduled_arrival AS first_arrival_time,
                              			  f.scheduled_departure AS second_deaparture_time,
                              			  f.scheduled_arrival AS second_arrival_time 
                               FROM flights f 
                               WHERE f.departure_airport = 'DME' AND f.arrival_airport = 'VKO' AND DATE(f.scheduled_departure) = '2017-09-09'
                              ), 

                              connecting_flights AS (
                               SELECT f1.flight_id AS first_flight, 
                                      f1.scheduled_departure, f2.scheduled_arrival, 1 AS connections, 
                                      LEAST( 
                                          ( 
                                              SELECT COUNT(*) 
                                              FROM seats s1 
                                              WHERE s1.aircraft_code = f1.aircraft_code 
                                                AND s1.seat_no NOT IN ( 
                                                    SELECT bp.seat_no 
                                                    FROM boarding_passes bp 
                                                    JOIN ticket_flights tf ON bp.ticket_no = tf.ticket_no 
                                                    WHERE tf.flight_id = f1.flight_id 
                                                ) 
                                          ), 
                                          ( 
                                              SELECT COUNT(*) 
                                              FROM seats s2 
                                              WHERE s2.aircraft_code = f2.aircraft_code 
                                                AND s2.seat_no NOT IN ( 
                                                    SELECT bp.seat_no 
                                                    FROM boarding_passes bp 
                                                    JOIN ticket_flights tf ON bp.ticket_no = tf.ticket_no 
                                                    WHERE tf.flight_id = f2.flight_id 
                                                ) 
                                         ) 
                                     ) AS free_seats, 
                                      (f2.scheduled_arrival - f1.scheduled_departure) AS total_duration, 
                              		  f1.flight_id AS first_flight_id,
                              		  f2.flight_id AS second_flight_id,
                              		  f1.aircraft_code AS first_aircraft_code,
                              		  f2.aircraft_code AS second_aircraft_code,
                              		  f1.scheduled_departure AS first_departure_time,
                              			  f1.scheduled_arrival AS first_arrival_time,
                              		  f2.scheduled_departure AS second_deaparture_time,
                              			  f2.scheduled_arrival AS second_arrival_time 
                               FROM flights f1 
                               JOIN flights f2 ON f1.arrival_airport = f2.departure_airport 
                               WHERE ((f2.scheduled_arrival - f1.scheduled_departure) <= INTERVAL '24 hours') 
                                 AND f1.departure_airport = 'DME' AND f2.arrival_airport = 'VKO' 
                                AND DATE (f1.scheduled_departure) = '2017-09-09'
								AND f2.scheduled_departure > f1.scheduled_arrival
                                AND f1.scheduled_departure < f2.scheduled_arrival 
                              ) 

                              (SELECT * FROM direct_flights WHERE free_seats > 0) 
                              UNION ALL 
                              (SELECT * FROM connecting_flights WHERE free_seats > 0) 
                              ORDER BY total_duration;