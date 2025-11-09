/*
* Created by Alejandro Zheng on 3/5/21.
*/
/*06/11/2025 resolver el problema de que el menu solo deja meter una parte de date*/

    /* abrir un fichero (file) e imprimir from , to, dateguaro en fichero los resultados de consulta
    compongo query_result_set con los resultados de la consulta en vez de guardarlos en fichero
    Conecto BD, preparo consulta, ejecuto, 
    */
#include "search.h"
#include <stdio.h>
#include <sql.h>
#include <sqlext.h>
void    results_search(char * from, char *to, char *date,/*añadir date*/
                       int * n_choices, char *** choices,
                       int max_length,
                       int max_rows)
   /**here you need to do your query and fill the choices array of strings
 *
 * @param from form field from
 * @param to form field to
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
    SQLCHAR first_flight[512];
    SQLCHAR scheduled_departure[512];
    SQLCHAR scheduled_arrival[512];
    SQLCHAR connections[512];
    SQLCHAR free_seats[512];
    SQLCHAR total_duration[512];

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return;
    }

       /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt, (SQLCHAR*) "WITH direct_flights AS ("
" SELECT f.flight_id AS first_flight, "
"        f.scheduled_departure, f.scheduled_arrival, 0 AS connections, "
"        ( "
"            SELECT COUNT(*) "
"            FROM seats s "
"            WHERE s.aircraft_code = f.aircraft_code "
"              AND s.seat_no NOT IN ( "
"                  SELECT bp.seat_no "
"                  FROM boarding_passes bp "
"                  JOIN ticket_flights tf ON bp.ticket_no = tf.ticket_no "
"                  WHERE tf.flight_id = f.flight_id "
"              ) "
"        ) AS free_seats, "
"        (f.scheduled_arrival - f.scheduled_departure) AS total_duration "
" FROM flights f "
" WHERE f.departure_airport = ? AND f.arrival_airport = ? AND DATE(f.scheduled_arrival) = ? "
"), "

"connecting_flights AS ("
" SELECT f1.flight_id AS first_flight, "
"        f1.scheduled_departure, f2.scheduled_arrival, 1 AS connections, "
"        LEAST( "
"            ( "
"                SELECT COUNT(*) "
"                FROM seats s1 "
"                WHERE s1.aircraft_code = f1.aircraft_code "
"                  AND s1.seat_no NOT IN ( "
"                      SELECT bp.seat_no "
"                      FROM boarding_passes bp "
"                      JOIN ticket_flights tf ON bp.ticket_no = tf.ticket_no "
"                      WHERE tf.flight_id = f1.flight_id "
"                  ) "
"            ), "
"            ( "
"                SELECT COUNT(*) "
"                FROM seats s2 "
"                WHERE s2.aircraft_code = f2.aircraft_code "
"                  AND s2.seat_no NOT IN ( "
"                      SELECT bp.seat_no "
"                      FROM boarding_passes bp "
"                      JOIN ticket_flights tf ON bp.ticket_no = tf.ticket_no "
"                      WHERE tf.flight_id = f2.flight_id "
"                  ) "
"            ) "
"        ) AS free_seats, "
"        (f2.scheduled_arrival - f1.scheduled_departure) AS total_duration "
" FROM flights f1 "
" JOIN flights f2 ON f1.arrival_airport = f2.departure_airport "
" WHERE ((f2.scheduled_arrival - f1.scheduled_departure) <= INTERVAL '24 hours') "
"   AND f1.departure_airport = ? AND f2.arrival_airport = ? "
"   AND DATE(f2.scheduled_arrival) = ? "
"   AND f1.scheduled_departure < f2.scheduled_arrival "
") "

"(SELECT * FROM direct_flights WHERE free_seats > 0) "
"UNION ALL "
"(SELECT * FROM connecting_flights WHERE free_seats > 0) "
"ORDER BY total_duration;", SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_C_LONG, 0, 0, from, 0, NULL);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_C_LONG, 0, 0, to, 0, NULL);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_C_LONG, 0, 0, date, 0, NULL);
    
    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_C_LONG, 0, 0, from, 0, NULL);
    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_C_LONG, 0, 0, to, 0, NULL);
    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_C_LONG, 0, 0, date, 0, NULL);
        
    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        fprintf(stderr, "Error ejecutando consulta SQL\n");
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        odbc_disconnect(env, dbc);
        return;
    }
    SQLBindCol(stmt, 1, SQL_C_CHAR, first_flight, sizeof(first_flight), NULL);/*modificar*/
    SQLBindCol(stmt, 2, SQL_C_CHAR, scheduled_departure, sizeof(scheduled_departure), NULL);
    SQLBindCol(stmt, 3, SQL_C_CHAR, scheduled_arrival, sizeof(scheduled_arrival), NULL);
    SQLBindCol(stmt, 4, SQL_C_CHAR, connections, sizeof(connections), NULL);
    SQLBindCol(stmt, 5, SQL_C_CHAR, free_seats, sizeof(free_seats), NULL);
    SQLBindCol(stmt, 6, SQL_C_CHAR, total_duration, sizeof(total_duration), NULL);


    /* Loop through the rows in the result-set */
    while (row < max_rows && SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        snprintf((*choices)[row],max_length,"%-4s  %-20s  %-20s  %s  %-3s  %-8s\n", first_flight,scheduled_departure,scheduled_arrival,connections,free_seats,total_duration);
        row++;
    }


    *n_choices = row;

    SQLCloseCursor(stmt);


    /* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return;
    }

    return;

  }

