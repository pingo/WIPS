CREATE TABLE `mote`
(
	`major` INTEGER, -- First part of the mote address
	`minor` INTEGER, -- Second part of the mote address
	`name`  TEXT,    -- A human readable name

	PRIMARY KEY (`major`, `minor`)
);

CREATE TABLE `event`
(
	`major`   INTEGER, -- Origin mote major ID
	`minor`   INTEGER, -- Origin mote minor ID
	`time`    INTEGER, -- Time of event
	`hops`    INTEGER, -- Number of hops to the sink
	`rssi`    INTEGER, -- Signal quality of the packet
	`seq`     INTEGER, -- Sequence bit
	`retries` INTEGER, -- Retries before receiving sink ACK packet
	`value`   INTEGER, -- Event data

	FOREIGN KEY (`major`, `minor`) REFERENCES `mote`(`major`, `minor`)
);

