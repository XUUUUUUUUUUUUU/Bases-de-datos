--Create the new table in order to give a different configuration of seats for each flights
CREATE TABLE public.flight_seat (
    flight_id integer NOT NULL,
    seat_no character varying(4) NOT NULL
);
ALTER TABLE public.flight_seat OWNER TO alumnodb;

--Add primay key to new table flight_seat
ALTER TABLE ONLY public.flight_seat
    ADD CONSTRAINT flight_seat_pkey PRIMARY KEY (flight_id, seat_no);

--Add foreign key to new table flight_seat
ALTER TABLE ONLY public.flight_seat
    ADD CONSTRAINT flight_seat_flight_id_fkey FOREIGN KEY (flight_id) REFERENCES public.flights(flight_id);
--Insertar datos en flight_seat
INSERT INTO flight_seat(flight_id,seat_no)
SELECT f.flight_id, s.seat_no
FROM flights f 
JOIN seats s ON f.aircraft_code=s.aircraft_code;