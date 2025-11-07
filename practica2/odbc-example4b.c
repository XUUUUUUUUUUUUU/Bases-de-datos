#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

/*
 * example 4 with a queries build on-the-fly, the good way
 */

int main(void) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    char x[512];
    SQLCHAR ticket_no[512];
    SQLCHAR flight_id[512];
    SQLCHAR boarding_no[512];
    SQLCHAR assigned_seat_no[512];


    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt, (SQLCHAR*) "WITH \
    bookings_ticket AS ( \
        SELECT \
            b.book_ref, \
            tf.ticket_no, \
            tf.flight_id, \
            f.scheduled_departure, \
            t.passenger_name, \
            f.aircraft_code, \
            ROW_NUMBER() OVER(PARTITION BY tf.flight_id ORDER BY tf.ticket_no) AS ticket_rank \
        FROM \
            bookings b \
            JOIN tickets t ON t.book_ref = b.book_ref \
            JOIN ticket_flights tf ON tf.ticket_no = t.ticket_no \
            JOIN flights f ON f.flight_id = tf.flight_id \
        WHERE \
            NOT EXISTS ( \
                SELECT 1 \
                FROM boarding_passes bp_check \
                WHERE bp_check.flight_id = tf.flight_id AND bp_check.ticket_no = tf.ticket_no \
            ) \
            AND b.book_ref = ? \
    ), \
    available_seats AS ( \
        SELECT \
            s.seat_no, \
            s.aircraft_code, \
            f.flight_id, \
            ROW_NUMBER() OVER(PARTITION BY f.flight_id ORDER BY s.seat_no) AS seat_rank \
        FROM \
            seats s \
            JOIN flights f ON s.aircraft_code = f.aircraft_code \
            WHERE f.flight_id IN (SELECT flight_id FROM bookings_ticket) \
            AND NOT EXISTS ( \
                SELECT 1 \
                FROM boarding_passes bp \
                WHERE bp.flight_id = f.flight_id AND bp.seat_no = s.seat_no \
            ) \
    ), \
    boarding_passes_last_boarding_no AS ( \
        SELECT \
            bp.ticket_no, \
            bp.flight_id, \
            MAX(bp.boarding_no) AS max_boarding_no \
        FROM boarding_passes bp \
        GROUP BY bp.ticket_no, bp.flight_id \
    ), \
    ticket_to_allocate AS ( \
        SELECT \
            bt.ticket_no, \
            bt.flight_id, \
            avs.seat_no AS assigned_seat_no, \
            COALESCE(bplbn.max_boarding_no, 0) + 1 AS new_boarding_no \
        FROM \
            bookings_ticket bt \
        JOIN \
            available_seats avs ON bt.flight_id = avs.flight_id AND bt.ticket_rank = avs.seat_rank \
        LEFT JOIN \
            boarding_passes_last_boarding_no bplbn ON bplbn.ticket_no = bt.ticket_no AND bplbn.flight_id = bt.flight_id \
    ) \
INSERT INTO \
    boarding_passes (ticket_no, flight_id, boarding_no, seat_no) \
SELECT \
    tta.ticket_no, \
    tta.flight_id, \
    tta.new_boarding_no, \
    tta.assigned_seat_no \
FROM \
    ticket_to_allocate tta \
RETURNING ticket_no, \
    flight_id, \
    boarding_no, \
    seat_no;", SQL_NTS);

    printf("x = ");
    fflush(stdout);
    while (scanf("%s", x) != EOF) {
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 6, 0, &x, (SQLINTEGER)sizeof(x), NULL);
        
        SQLExecute(stmt);
        
        SQLBindCol(stmt, 1, SQL_C_CHAR, ticket_no, sizeof(ticket_no), NULL);
        SQLBindCol(stmt, 2, SQL_C_CHAR, flight_id, sizeof(flight_id), NULL);
        SQLBindCol(stmt, 3, SQL_C_CHAR, boarding_no, sizeof(boarding_no), NULL);
        SQLBindCol(stmt, 4, SQL_C_CHAR, assigned_seat_no, sizeof(assigned_seat_no), NULL);

        /* Loop through the rows in the result-set */
        while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            printf("ticket_no = %s\n", ticket_no);
            printf("flight_id = %s\n", flight_id);
            printf("boarding_no = %s\n", boarding_no);
            printf("assigned_seat_no = %s\n", assigned_seat_no);
        }

        SQLCloseCursor(stmt);

        printf("x = ");
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

