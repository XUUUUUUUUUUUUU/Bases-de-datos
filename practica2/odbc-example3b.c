#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

/*
 * example 3 with a queries build on-the-fly, the bad way
 */

int main(void) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    char book_ref[512];
    SQLCHAR passenger_name[512];
    SQLCHAR flight_id[512];
    SQLCHAR scheduled_departure[512];
    SQLCHAR assigned_seat_no[512];


    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    printf("book_ref = ");
    fflush(stdout);
    while (fgets(book_ref, sizeof(book_ref), stdin) != NULL) {
        book_ref[strcspn(book_ref, "\n")] = 0;
        char query[3000];
        sprintf(query, "WITH bookings_ticket AS ( \
    SELECT \
        b.book_ref, \
        tf.ticket_no, \
        tf.flight_id, \
        f.scheduled_departure, \
        t.passenger_name, \
        f.aircraft_code, \
        ROW_NUMBER() OVER (PARTITION BY tf.flight_id ORDER BY tf.ticket_no) AS ticket_rank \
    FROM \
        bookings b \
        JOIN tickets t ON t.book_ref = b.book_ref \
        JOIN ticket_flights tf ON tf.ticket_no = t.ticket_no \
        JOIN flights f ON f.flight_id = tf.flight_id \
    WHERE \
        NOT EXISTS ( \
            SELECT 1 \
            FROM boarding_passes bp_check \
            WHERE bp_check.flight_id = tf.flight_id \
              AND bp_check.ticket_no = tf.ticket_no \
        ) \
      AND b.book_ref = '%s' \
), \
available_seats AS ( \
    SELECT \
        s.seat_no, \
        s.aircraft_code, \
        f.flight_id, \
        ROW_NUMBER() OVER (PARTITION BY f.flight_id ORDER BY s.aircraft_code, s.seat_no) AS seat_rank \
    FROM seats s \
    JOIN flights f ON s.aircraft_code = f.aircraft_code \
    WHERE f.flight_id IN (SELECT flight_id FROM bookings_ticket) \
      AND NOT EXISTS ( \
            SELECT 1 \
            FROM boarding_passes bp \
            WHERE bp.flight_id = f.flight_id \
              AND bp.seat_no = s.seat_no \
      ) \
), \
boarding_passes_last_boarding_no AS ( \
    SELECT \
        bp.flight_id, \
        COALESCE(MAX(bp.boarding_no), 0) AS max_boarding_no \
    FROM boarding_passes bp \
    GROUP BY bp.flight_id \
), \
ticket_to_allocate AS ( \
    SELECT \
        bt.ticket_no, \
        bt.flight_id, \
        avs.seat_no AS assigned_seat_no, \
        COALESCE(bplbn.max_boarding_no, 0) + bt.ticket_rank AS new_boarding_no \
    FROM bookings_ticket bt \
    JOIN available_seats avs ON bt.flight_id = avs.flight_id AND bt.ticket_rank = avs.seat_rank \
    LEFT JOIN boarding_passes_last_boarding_no bplbn ON bplbn.flight_id = bt.flight_id \
), \
ins AS ( \
INSERT INTO boarding_passes (ticket_no, flight_id, boarding_no, seat_no) \
    SELECT \
        tta.ticket_no, \
        tta.flight_id, \
        tta.new_boarding_no, \
        tta.assigned_seat_no \
    FROM ticket_to_allocate tta \
    ON CONFLICT (flight_id, seat_no) DO NOTHING \
    RETURNING ticket_no, flight_id, boarding_no, seat_no \
) \
SELECT \
    SUBSTRING(t.passenger_name FROM 1 FOR 20) AS passenger_name, \
    i.flight_id, \
    f.scheduled_departure, \
    i.seat_no \
FROM ins i \
JOIN tickets t ON t.ticket_no = i.ticket_no \
JOIN flights f ON f.flight_id = i.flight_id \
ORDER BY i.ticket_no ASC;", book_ref); /*abres un *file, imprimes la consulta, make consulta*/
        SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

        SQLBindCol(stmt, 1, SQL_C_CHAR, passenger_name, sizeof(passenger_name), NULL);
        SQLBindCol(stmt, 2, SQL_C_CHAR, flight_id, sizeof(flight_id), NULL);
        SQLBindCol(stmt, 3, SQL_C_CHAR, scheduled_departure, sizeof(scheduled_departure), NULL);
        SQLBindCol(stmt, 4, SQL_C_CHAR, assigned_seat_no, sizeof(assigned_seat_no), NULL);


        /* Loop through the rows in the result-set */
        while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            printf("%s flight_id = %s scheduled_departure= %s seat_no = %s\n",passenger_name,
                                                                                flight_id,
                                                                                scheduled_departure,
                                                                                assigned_seat_no);
        }

        SQLCloseCursor(stmt); /* OJO: Limpio la variable stmt*/

        printf("book_ref = ");
        fflush(stdout);
    }
    printf("\n");
    
    /* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

