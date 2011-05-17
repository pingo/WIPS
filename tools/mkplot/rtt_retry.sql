--
-- Retrieve packet round trip time in the network
--

-- SELECT AVG(`rtt`) FROM
-- (
	SELECT
		`n1`.`address`,
		`n1`.`timestamp` AS `tx_time`, (`n2`.`timestamp` - `n1`.`timestamp`) AS `rtt`,
		MAX(`n1`.`retries`) AS `tx_retries`
	FROM
		(SELECT MIN(`timestamp`) AS `timestamp`, COUNT(*) - 1 AS `retries`, `address`, `seqno` FROM `nodes` WHERE `p_type` = 'tx' GROUP BY `address`, `seqno`) AS `n1`
		INNER JOIN `nodes` AS `n2` USING (`address`, `seqno`)
	WHERE
		`n2`.`p_type` = 'rx'
	GROUP BY
		`n1`.`address`, `n1`.`seqno`;
-- );

