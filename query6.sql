WITH retraso_medio
     AS (SELECT f.flight_no,
                Avg(f.actual_arrival - f.scheduled_arrival) AS retraso
         FROM   flights f
         GROUP  BY f.flight_no)
SELECT rmedio.flight_no,
       rmedio.retraso
FROM   retraso_medio rmedio
WHERE  rmedio.retraso = (SELECT Max(retraso)
                         FROM   retraso_medio); 