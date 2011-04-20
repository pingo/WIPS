import serial, argparse, sqlite3

def serve(db, port):
	s = serial.Serial(port, 115200)

	s.readline() # Skip one line to prevent fragmented line.

	for line in s:
		line = line.rstrip('\r\n')
		addr, v, hops = line.split(' ')

		major, minor = addr.split('.')

		t = (
				int(major),
				int(minor)
			)

		db.execute('INSERT OR IGNORE INTO `mote` (`major`, `minor`) VALUES (?, ?)', t)

		t = (
				int(major),
				int(minor),
				int(hops),
				int(v)
			)

		db.execute('INSERT INTO `event` VALUES (?, ?, strftime(\'%s\', \'now\'), ?, ?)', t)
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

