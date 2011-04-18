import serial, argparse, sqlite3

def serve(db, port):
	s = serial.Serial(port, 115200)
	c = db.cursor()

	s.readline() # Skip one line to prevent fragmented line.

	for line in s:
		line = line.rstrip('\r\n')
		saddr, v, laddr, seqno, hops = line.split(' ')

		saddr_major, saddr_minor = saddr.split('.')
		laddr_major, laddr_minor = saddr.split('.')

		t = [
				(int(saddr_major), int(saddr_minor)),
			 	(int(laddr_major), int(laddr_minor))
			]

		c.executemany('INSERT OR IGNORE INTO `mote` (`major`, `minor`) VALUES (?, ?)', t)

		t = (
				int(saddr_major),
				int(saddr_minor),
				int(laddr_major),
				int(laddr_minor),
				int(v)
			)

		c.execute('INSERT INTO `event` VALUES (?, ?, ?, ?, strftime(\'%s\', \'now\'), ?)', t)
		db.commit()

def main():
	argumentparser = argparse.ArgumentParser(description='Presence sensing storage server')
	argumentparser.add_argument('db', metavar='DATABASE', type=str, help='database file to use')
	argumentparser.add_argument('port', metavar='PORT', type=str, help='serial port to monitor')

	arguments = argumentparser.parse_args()

	db = sqlite3.connect(arguments.db)

	serve(db, arguments.port)
	db.close()

if __name__ == '__main__':
	main()

