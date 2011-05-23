import serial, argparse, sqlite3, csv
import SocketServer
from SimpleXMLRPCServer import SimpleXMLRPCServer
from SimpleXMLRPCServer import SimpleXMLRPCRequestHandler
from threading import Thread, Lock, Event
from time import time

s       = None    # Serial port file object.
q       = {}      # Queued ACKs.
qLock   = Lock()  # Lock for ACK queue.
uEvent  = Event() # New data event.
uLatest = time();

UPDATE_TIMEOUT = 5.0  # How long to wait for ACKs.
DATA_TIMEOUT = 10.0   # How long to block when waiting for new data.

class AsyncXMLRPCServer(SocketServer.ThreadingMixIn,SimpleXMLRPCServer): pass

class RequestHandler(SimpleXMLRPCRequestHandler):
    rpc_paths = ('/RPC2',)

def queueAck(addr, t):
	acksLock.acquire()
	l = acks.setdefault(addr, [])
	l.append(Ack(t))
	acksLock.release()

def updateValue(t, addr, value):
	s.write(str(addr) + ':' + t + str(value) + '\r\n')

	qLock.acquire()
	acks = q.setdefault(addr, {})

	# If the delta is already being set by somebody else, we fail.
	if t in acks:
		qLock.release()
		return False

	# Create a new event to wait for.
	event = Event()

	acks[t] = event
	qLock.release()

	success = event.wait(UPDATE_TIMEOUT)

	qLock.acquire()
	del acks[t]
	qLock.release()

	return success


class ServerThread(Thread):
	def __init__(self):
		Thread.__init__(self)

	def rpcSetDelta(self, addr, value):
		return updateValue('d', addr, value)

	def rpcSetSensorTimeout(self, addr, value):
		return updateValue('s', addr, value)

	def rpcSetSensorInterval(self, addr, value):
		return updateValue('i', addr, value)

	def rpcSetBeaconInterval(self, addr, value):
		return updateValue('b', value)

	def rpcWaitForData(self, time):
		global uEvent, uLatest

		print "uLatest:", uLatest, ", time:", time

		if uLatest >= time:
			return True

		success = uEvent.wait(DATA_TIMEOUT)
		uEvent.clear()
		return success

	def run(self):
		print "Starting server..."

		self.server = AsyncXMLRPCServer(("localhost", 8000), requestHandler=RequestHandler)
		self.server.register_introspection_functions()
		self.server.register_multicall_functions()
		self.server.register_function(self.rpcSetDelta, 'setDelta')
		self.server.register_function(self.rpcSetSensorTimeout, 'setSensorTimeout')
		self.server.register_function(self.rpcSetSensorInterval, 'setSensorInterval')
		self.server.register_function(self.rpcSetBeaconInterval, 'setBeaconInterval')
		self.server.register_function(self.rpcWaitForData, 'waitForData')

		self.server.serve_forever()

server = ServerThread()
server.start()

def serve(db, port):
	global s

	s = serial.Serial(port, 115200)
	# Skip one line to prevent fragmented line.
	s.readline() 

	reader = csv.reader(s, delimiter=',')

	for line in reader:
		if line[0] == 't':
			typ, addr, hops, rssi, seq_flag, retries, v = line

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
					int(rssi),
					int(seq_flag),
					int(retries),
					int(v)
				)

			db.execute('INSERT INTO `event` VALUES (?, ?, strftime(\'%s\', \'now\'), ?, ?, ?, ?, ?)', t)
			db.commit()

			print line

			uLatest = time()
			uEvent.set() # New data has arrived.

		else:
			typ = line[0]
			addr = line[1]

			print 'Recieved ACK type', typ, 'from', addr

			qLock.acquire()

			if addr in q:
				e = q[addr]
				e[typ].set()

				print 'Success'
			else:
				print 'Unqueued ACK received from', addr

			qLock.release()

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

