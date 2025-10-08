WITH retraso_medio AS(
    SELECT f.flight_id,AVG(f.actual_arrival-f.scheduled_arrival) AS retraso
    FROM flights f
    GROUP BY f.flight_id
)
SELECT rmedio.flight_id, rmedio.retraso
FROM retraso_medio rmedio
WHERE rmedio.retraso=(SELECT MAX(retraso) FROM retraso_medio);