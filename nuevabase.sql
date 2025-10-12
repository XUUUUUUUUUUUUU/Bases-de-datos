CREATE TABLE public.aircrafts_data (
    aircraft_code character(3) NOT NULL,
    model jsonb NOT NULL,
    range integer NOT NULL,
    CONSTRAINT aircrafts_range_check CHECK ((range > 0))
);
ALTER TABLE public.aircrafts_data OWNER TO alumnodb;

CREATE TABLE public.airports_data (
    airport_code character(3) NOT NULL,
    airport_name jsonb NOT NULL,
    city jsonb NOT NULL,
    coordinates point NOT NULL,
    timezone text NOT NULL
);
ALTER TABLE public.airports_data OWNER TO alumnodb;

CREATE TABLE public.boarding_passes (
    ticket_no character(13) NOT NULL,
    flight_id integer NOT NULL,
    boarding_no integer NOT NULL,
    seat_no character varying(4) NOT NULL
);
ALTER TABLE public.boarding_passes OWNER TO alumnodb;


CREATE TABLE public.bookings (
    book_ref character(6) NOT NULL,
    book_date timestamp with time zone NOT NULL,
    total_amount numeric(10,2) NOT NULL
);
ALTER TABLE public.bookings OWNER TO alumnodb;

CREATE TABLE public.flights (
    flight_id integer NOT NULL,
    flight_no character(6) NOT NULL,
    scheduled_departure timestamp with time zone NOT NULL,
    scheduled_arrival timestamp with time zone NOT NULL,
    departure_airport character(3) NOT NULL,
    arrival_airport character(3) NOT NULL,
    status character varying(20) NOT NULL,
    aircraft_code character(3) NOT NULL,
    actual_departure timestamp with time zone,
    actual_arrival timestamp with time zone,
    CONSTRAINT flights_check CHECK ((scheduled_arrival > scheduled_departure)),
    CONSTRAINT flights_check1 CHECK (((actual_arrival IS NULL) OR ((actual_departure IS NOT NULL) AND (actual_arrival IS NOT NULL) AND (actual_arrival > actual_departure)))),
    CONSTRAINT flights_status_check CHECK (((status)::text = ANY (ARRAY[('On Time'::character varying)::text, ('Delayed'::character varying)::text, ('Departed'::character varying)::text, ('Arrived'::character varying)::text, ('Scheduled'::character varying)::text, ('Cancelled'::character varying)::text])))
);
ALTER TABLE public.flights OWNER TO alumnodb;


CREATE SEQUENCE public.flights_flight_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

CREATE TABLE public.seats (
    aircraft_code character(3) NOT NULL,
    seat_no character varying(4) NOT NULL,
    fare_conditions character varying(10) NOT NULL,
    CONSTRAINT seats_fare_conditions_check CHECK (((fare_conditions)::text = ANY (ARRAY[('Economy'::character varying)::text, ('Comfort'::character varying)::text, ('Business'::character varying)::text])))
);
ALTER TABLE public.seats OWNER TO alumnodb;


CREATE TABLE public.ticket_flights (
    ticket_no character(13) NOT NULL,
    flight_id integer NOT NULL,
    fare_conditions character varying(10) NOT NULL,
    amount numeric(10,2) NOT NULL,
    CONSTRAINT ticket_flights_amount_check CHECK ((amount >= (0)::numeric)),
    CONSTRAINT ticket_flights_fare_conditions_check CHECK (((fare_conditions)::text = ANY (ARRAY[('Economy'::character varying)::text, ('Comfort'::character varying)::text, ('Business'::character varying)::text])))
);
ALTER TABLE public.ticket_flights OWNER TO alumnodb;

CREATE TABLE public.tickets (
    ticket_no character(13) NOT NULL,
    book_ref character(6) NOT NULL,
    passenger_id character varying(20) NOT NULL,
    passenger_name text NOT NULL,
    contact_data jsonb
);
ALTER TABLE public.tickets OWNER TO alumnodb;

--Create the new table in order to give a different configuration of seats for each flights
CREATE TABLE public.flight_seat (
    flight_id integer NOT NULL,
    seat_no character varying(4) NOT NULL
);
ALTER TABLE public.flight_seat OWNER TO alumnodb;

--Asign the primary keys and constraint
ALTER TABLE ONLY public.aircrafts_data
    ADD CONSTRAINT aircrafts_pkey PRIMARY KEY (aircraft_code);

ALTER TABLE ONLY public.airports_data
    ADD CONSTRAINT airports_data_pkey PRIMARY KEY (airport_code);


ALTER TABLE ONLY public.boarding_passes
    ADD CONSTRAINT boarding_passes_flight_id_boarding_no_key UNIQUE (flight_id, boarding_no);

ALTER TABLE ONLY public.boarding_passes
    ADD CONSTRAINT boarding_passes_flight_id_seat_no_key UNIQUE (flight_id, seat_no);

ALTER TABLE ONLY public.boarding_passes
    ADD CONSTRAINT boarding_passes_pkey PRIMARY KEY (ticket_no, flight_id);

ALTER TABLE ONLY public.bookings
    ADD CONSTRAINT bookings_pkey PRIMARY KEY (book_ref);

ALTER TABLE ONLY public.flights
    ADD CONSTRAINT flights_flight_no_scheduled_departure_key UNIQUE (flight_no, scheduled_departure);

ALTER TABLE ONLY public.flights
    ADD CONSTRAINT flights_pkey PRIMARY KEY (flight_id);

ALTER TABLE ONLY public.seats
    ADD CONSTRAINT seats_pkey PRIMARY KEY (aircraft_code, seat_no);

ALTER TABLE ONLY public.seats
    ADD CONSTRAINT seats_pkey PRIMARY KEY (aircraft_code, seat_no);

ALTER TABLE ONLY public.ticket_flights
    ADD CONSTRAINT ticket_flights_pkey PRIMARY KEY (ticket_no, flight_id);

ALTER TABLE ONLY public.tickets
    ADD CONSTRAINT tickets_pkey PRIMARY KEY (ticket_no);

--Add primay key to new table flight_seat
ALTER TABLE ONLY public.flight_seat
    ADD CONSTRAINT flight_seat_pkey PRIMARY KEY (flight_id, seat_no);

--Assign the foreign keys
ALTER TABLE ONLY public.boarding_passes
    ADD CONSTRAINT boarding_passes_ticket_no_fkey FOREIGN KEY (ticket_no, flight_id) REFERENCES public.ticket_flights(ticket_no, flight_id);

ALTER TABLE ONLY public.flights
    ADD CONSTRAINT flights_aircraft_code_fkey FOREIGN KEY (aircraft_code) REFERENCES public.aircrafts_data(aircraft_code);

ALTER TABLE ONLY public.flights
    ADD CONSTRAINT flights_arrival_airport_fkey FOREIGN KEY (arrival_airport) REFERENCES public.airports_data(airport_code);

ALTER TABLE ONLY public.flights
    ADD CONSTRAINT flights_departure_airport_fkey FOREIGN KEY (departure_airport) REFERENCES public.airports_data(airport_code);

ALTER TABLE ONLY public.seats
    ADD CONSTRAINT seats_aircraft_code_fkey FOREIGN KEY (aircraft_code) REFERENCES public.aircrafts_data(aircraft_code) ON DELETE CASCADE;

ALTER TABLE ONLY public.ticket_flights
    ADD CONSTRAINT ticket_flights_flight_id_fkey FOREIGN KEY (flight_id) REFERENCES public.flights(flight_id);

ALTER TABLE ONLY public.ticket_flights
    ADD CONSTRAINT ticket_flights_ticket_no_fkey FOREIGN KEY (ticket_no) REFERENCES public.tickets(ticket_no);

ALTER TABLE ONLY public.tickets
    ADD CONSTRAINT tickets_book_ref_fkey FOREIGN KEY (book_ref) REFERENCES public.bookings(book_ref);

--Add foreign key to new table flight_seat
ALTER TABLE ONLY public.flight_seat
    ADD CONSTRAINT flight_seat_flight_id_fkey FOREIGN KEY (flight_id) REFERENCES public.flights(flight_id);
