#!/usr/bin/env python2

import sys, os, csv, argparse, sqlite3


def read_data(infile, node_id, node_type, db):
    # Make CSV objects
    csvreader = csv.reader(infile, skipinitialspace=True, delimiter=' ')

    c = db.cursor()
    # Create table
    c.execute("create table if not exists nodes (address text, timestamp integer, p_type text, seqno integer, retries integer, hops integer, rssi integer)")
    c.execute("create table if not exists sink  (address text, timestamp integer, seqno integer, retries integer, hops integer, rssi integer)")

    # Parser for sensor node
    if node_type == 'node':
        for row in csvreader:
            if row[1] == 't':
                c.execute("insert into nodes values (?,?,?,?,?,NULL,NULL)", (node_id, row[0], 'tx', row[3], row[4]))
            elif row[1] == 'r':
                c.execute("insert into nodes values (?,?,?,?,NULL,?,?)", (node_id, row[0], 'rx', row[5], row[3], row[4]))
    else:
        for row in csvreader:
            c.execute("insert into sink values (?,?,?,?,?,?)", (row[1], row[0], row[4], row[5], row[2], row[3]))

    db.commit()

def main():
    argumentparser = argparse.ArgumentParser(description='Parse serialdump data and inject into sqlite db')
    argumentparser.add_argument('dbfile', metavar='DATABASE', help='target sqlite database')
    argumentparser.add_argument('node_id', metavar='NODEID', help='node id of serialdump file')
    argumentparser.add_argument('node_type', metavar='NODETYPE', choices=['sink', 'node'], help='type of data: sink or sensor node')
    argumentparser.add_argument('ifile', metavar='INPUT', type=argparse.FileType('rb', 1), help='input serialdump file')

    arguments = argumentparser.parse_args()

    db = sqlite3.connect(arguments.dbfile)
    read_data(arguments.ifile, arguments.node_id, arguments.node_type, db)

    db.close()
    arguments.ifile.close()

if __name__ == '__main__':
    main()

