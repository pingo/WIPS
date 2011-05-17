#!/bin/bash

sqlite3 -list -separator ' ' -batch db > rtt_retry.txt < rtt_retry.sql
sqlite3 -list -separator ' ' -batch db > rtt_perfect.txt < rtt_perfect.sql

/usr/bin/gnuplot plot.gnuplot

