CREATE TABLE `mote`
(
	`major` INTEGER, -- First part of the mote address
	`minor` INTEGER, -- Second part of the mote address
	`name`  TEXT,    -- A human readable name

	PRIMARY KEY (`major`, `minor`)
);

CREATE TABLE `event`
(
	`mote_major`  INTEGER, -- Origin mote major ID
	`mote_minor`  INTEGER, -- Origin mote minor ID
	`relay_major` INTEGER, -- Last relay mote major ID
	`relay_minor` INTEGER, -- Last relay mote minor ID
	`time`  INTEGER,       -- Time of event
	`value` INTEGER,       -- Event data

	FOREIGN KEY (`mote_major`, `mote_minor`) REFERENCES `mote`(`major`, `minor`),
	FOREIGN KEY (`relay_major`, `relay_minor`) REFERENCES `mote`(`major`, `minor`)
);

