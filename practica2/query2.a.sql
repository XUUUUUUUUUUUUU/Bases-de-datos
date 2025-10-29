INSERT INTO boarding_passes (
    ticket_no, 
    flight_id, 
    boarding_no, 
    seat_no,
    -- Debe incluir todos los campos NO NULOS requeridos, como la hora.
    boarding_time 
)
WITH flight_last_assigned_seat_no AS(
    -- CTE 1: Encontrar el asiento máximo asignado (MAX())
    SELECT bp.flight_id, MAX(bp.seat_no) AS last_seat_no
    FROM boarding_passes bp
    GROUP BY bp.flight_id
), 
bookings_ticket AS ( 
    -- CTE 2: Identificar los tickets sin asiento
    SELECT b.book_ref, tf.ticket_no, tf.flight_id, f.scheduled_departure, t.passenger_name
    FROM bookings b 
    JOIN tickets t ON t.book_ref = b.book_ref
    JOIN ticket_flights tf ON tf.ticket_no = t.ticket_no
    JOIN flights f ON f.flight_id = tf.flight_id
    WHERE NOT EXISTS (
        SELECT 1 FROM boarding_passes bp_check 
        WHERE bp_check.flight_id = tf.flight_id AND bp_check.ticket_no = tf.ticket_no
    ) 
    AND b.book_ref = 'USER_INPUT' 
),
boarding_passes_last_boarding_no AS (
    -- CTE 3: Encontrar el máximo boarding_no por ticket y vuelo
    SELECT 
        bp.ticket_no,
        bp.flight_id,
        MAX(bp.boarding_no) AS max_boarding_no
    FROM boarding_passes bp
    GROUP BY bp.ticket_no, bp.flight_id
),
ticket_to_allocate AS (
    -- CTE 4: Identificar al primer pasajero y calcular su asiento (MAX() + 1)
    SELECT 
        bt.ticket_no, 
        bt.flight_id,
        -- Asignar el nuevo asiento MAX() + 1
        (flasn.last_seat_no + 1) AS assigned_seat_no, 
        -- Calcular el nuevo boarding_no
        COALESCE(bplbn.max_boarding_no, 0) + 1 AS new_boarding_no
    FROM bookings_ticket bt
    JOIN flight_last_assigned_seat_no flasn ON flasn.flight_id = bt.flight_id
    -- LEFT JOIN para obtener el último boarding_no (puede que no exista si es la primera asignación)
    LEFT JOIN boarding_passes_last_boarding_no bplbn 
        ON bplbn.ticket_no = bt.ticket_no AND bplbn.flight_id = bt.flight_id
    -- Restricción CLAVE: Solo seleccionamos el ticket_no con la prioridad más alta (el MIN)
    WHERE bt.ticket_no = (
        SELECT MIN(ticket_no) 
        FROM bookings_ticket 
        WHERE flight_id = bt.flight_id
    )
)
-- ******************************************************
-- ** EJECUCIÓN DEL INSERT FINAL **
-- ******************************************************
SELECT 
    tta.ticket_no, 
    tta.flight_id,
    tta.new_boarding_no, -- boarding_no
    tta.assigned_seat_no -- seat_no
FROM ticket_to_allocate tta;