--Create the new table in order to give a different configuration of seats for each flights
CREATE TABLE
    public.flight_seat (
        flight_id integer NOT NULL,
        seat_no character varying(4) NOT NULL,
        fare_conditions character varying(10) NOT NULL,
        CONSTRAINT seats_fare_conditions_check CHECK (((fare_conditions)::text = ANY (ARRAY[('Economy'::character varying)::text, ('Comfort'::character varying)::text, ('Business'::character varying)::text])))
    );

ALTER TABLE public.flight_seat OWNER TO alumnodb;

--Add primay key to new table flight_seat
ALTER TABLE ONLY public.flight_seat ADD CONSTRAINT flight_seat_pkey PRIMARY KEY (flight_id, seat_no);

--Add foreign key to new table flight_seat
ALTER TABLE ONLY public.flight_seat ADD CONSTRAINT flight_seat_flight_id_fkey FOREIGN KEY (flight_id) REFERENCES public.flights (flight_id);

--Insertar datos en flight_seat
INSERT INTO
    flight_seat (flight_id, seat_no,fare_conditions)
SELECT
    f.flight_id,
    s.seat_no,
    fare_conditions
FROM
    flights f
    JOIN seats s ON f.aircraft_code = s.aircraft_code;