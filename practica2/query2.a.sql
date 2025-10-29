WITH flight_available_seat_no AS( 
    SELECT f.flight_id,f.aircraft_code,s.seat_no
    FROM flights f
    JOIN seat_ordered s ON s.aircraft_code=f.aircraft_code
    WHERE (f.flight_id,s.seat_no) NOT IN(
        SELECT flight_id,seat_no
        FROM boarding_passes
    )
),flight_last_assigned_seat_no AS(/* Buscamos el maximo seat no del asiento que ya esta asignado, luego asignamos el maximo +1 que si que esta libre da igual si hay */
    SELECT bp.flight_id,MAX(bp.seat_no) AS last_seat_no
    FROM boarding_passes bp
    GROUP BY bp.flight_id
) 
bookings_ticket AS ( /*Los booking que no tienen asientos asignados*/
    SELECT b.book_ref,tf.ticket_no,tf.flight_id,f.scheduled_departure,t.passenger_name
    FROM booking b
    JOIN ticket t ON t.book_ref=b.book_ref
    JOIN ticket_flights tf ON tf.ticket_no=t.ticket_no
    JOIN flights f ON f.flight_id=tf.flight_id
    WHERE (tf.flight_id,tf.ticket_no) NOT IN(
        SELECT flight_id,ticket_no
        FROM boarding_passes
    ) AND b.book_ref='USER_INPUT'
)
SELECT bt.book_ref, bt.flight_id,
        bt.scheduled_departure,SUBSTR(bt.passenger_name,1,20) AS passenger_name,
        flasn.seat_no+1
FROM booking_ticket bt 
JOIN flight_last_assigned_seat_no flasn ON flasn.flight_id=bt.flight_id
ORDER BY bt.ticket_no ASC;