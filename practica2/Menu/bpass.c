/*
 * Created by Shaofan Xu 07/11/2025
 */
#include "lbpass.h"
#include <sql.h>
#include <sqlext.h>
void results_bpass(/*@unused@*/ char *bookID,
                   int *n_choices, char ***choices,
                   int max_length,
                   int max_rows)
/**here you need to do your query and fill the choices array of strings
 *
 * @param bookID  form field bookId
 * @param n_choices fill this with the number of results
 * @param choices fill this with the actual results
 * @param max_length output win maximum width
 * @param max_rows output win maximum number of rows
 */

{
    int row = 0;
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    char passenger_name[512];
    char flight_id[512];
    char scheduled_departure[512];
    char assigned_seat_no[512];
    SQLLEN ind;

    *n_choices = 0;

    /*parameters check */
    if (bookID == NULL || *bookID == '\0')
    {
        if (max_rows > 0)
        {
            snprintf((*choices)[0], (size_t)max_length, "book_ref is empty");
            (*choices)[0][max_length - 1] = '\0';
            *n_choices = 1;
        }
        return;
    }

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);

    if (!SQL_SUCCEEDED(ret))
    {
        if (max_rows > 0)
        {
            snprintf((*choices)[0], (size_t)max_length, "ODBC connect failed");
            (*choices)[0][max_length - 1] = '\0';
            *n_choices = 1;
        }
        return;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt, (SQLCHAR *)"WITH bookings_ticket AS ( \
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
      AND b.book_ref = ? \
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
ORDER BY i.ticket_no ASC;",
               SQL_NTS);

    ind = SQL_NTS;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, (SQLULEN)strlen(bookID), 0, (SQLPOINTER)bookID, 0, &ind);

    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret))
    {
        if (max_rows > 0)
        {
            snprintf((*choices)[0], (size_t)max_length, "Query failed for book_ref=%s", bookID);
            (*choices)[0][max_length - 1] = '\0';
            *n_choices = 1;
        }
        SQLCloseCursor(stmt);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        odbc_disconnect(env, dbc);
        return;
    }

    SQLBindCol(stmt, 1, SQL_C_CHAR, passenger_name, sizeof(passenger_name), NULL);
    SQLBindCol(stmt, 2, SQL_C_CHAR, flight_id, sizeof(flight_id), NULL);
    SQLBindCol(stmt, 3, SQL_C_CHAR, scheduled_departure, sizeof(scheduled_departure), NULL);
    SQLBindCol(stmt, 4, SQL_C_CHAR, assigned_seat_no, sizeof(assigned_seat_no), NULL);

    /* Loop through the rows in the result-set */
    while (row < max_rows && SQL_SUCCEEDED(ret = SQLFetch(stmt)))
    {
        snprintf((*choices)[row], max_length, "%-20s %s %s %s", passenger_name, flight_id, scheduled_departure, assigned_seat_no);
        row++;
    }
    if (row == 0 && max_rows > 0)
    {
        snprintf((*choices)[0], (size_t)max_length, "No boarding passes assigned for %s", bookID);
        *n_choices = 1;
    }
    else
    {
        *n_choices = row;
    }

    SQLCloseCursor(stmt);
    /* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret))
    {
        return;
    }
}
