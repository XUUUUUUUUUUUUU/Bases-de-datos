WITH average_delay
     AS (SELECT Avg(f.actual_arrival - f.scheduled_arrival) AS retraso_medio
         FROM   flights f
         GROUP  BY f.flight_no)
SELECT f.flight_no,
       retraso_medio
FROM   flights f,
       average_delay
WHERE  retraso_medio = (SELECT Max(retraso_medio)
                        FROM   average_delay); 