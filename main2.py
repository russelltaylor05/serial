#!/usr/bin/python
import re
import serial
import time
import io
from time import sleep
import MySQLdb as mdb
import sys

con = None
try:
    con = mdb.connect('localhost', 'root', 'password', 'serial');
    cur = con.cursor()
    cur.execute("SELECT VERSION()")
    data = cur.fetchone()    
    print "Database version : %s " % data
    
except mdb.Error, e:  
    print "Error %d: %s" % (e.args[0],e.args[1])
    sys.exit(1)

#clear table
with con:
  cur = con.cursor()
  cur.execute("DELETE FROM info")


arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
time.sleep(2) #arduino resets when we connects so sleep for a little bit

while True:
  wg = arduino.write('T')
  line = arduino.readline().rstrip("\n")
  matchObj = re.search('\d\d\.\d', line)
  value = matchObj.group()
  print value
  with con:
    cur = con.cursor()
    cur.execute("INSERT INTO info(temp) VALUES('" + value + "')")

  time.sleep(4)

arduino.close()




