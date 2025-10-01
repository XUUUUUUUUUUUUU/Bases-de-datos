SELECT a.airport_code, COUNT(*) AS total_boardings
FROM boarding_passes bp
JOIN flights f ON bp.flight_id = f.flight_id
JOIN airport_data a ON f.arrival_airport = a.airport_code
GROUP BY a.airport_code
ORDER BY total_boardings ASC;
