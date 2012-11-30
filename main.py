#!/usr/bin/python
import re
import serial
import time
import io
from time import sleep
import MySQLdb as mdb
import sys
import json

# database credentials
host = 'localhost'
username = 'root'
password = 'password'
database = 'serial'



con = None
con = mdb.connect(host, username, password, database);


with con:
    cur = con.cursor()
    # drop table and recreat it.
    cur.execute("DROP TABLE info")
    cur.execute("CREATE TABLE IF NOT EXISTS \
                 info(id INT PRIMARY KEY AUTO_INCREMENT, \
                 temp DECIMAL(6,2) UNSIGNED, \
                 humidity DECIMAL(6,2) UNSIGNED, \
                 pressure DECIMAL(6,2) UNSIGNED, \
                 light DECIMAL(6,2) UNSIGNED, \
                 time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP)")


arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
time.sleep(2) #arduino resets when we connects so sleep for a little bit

while True:
  wg = arduino.write('H')
  time.sleep(3);
  wg = arduino.write('T')
  line = arduino.readline().rstrip("\n")
  values = json.loads(line);
  print line
    
  temp = float(values['data']['temp'].strip('"')) / 100;
  light = float(values['data']['light'].strip('"'));
  humidity = float(values['data']['humidity'].strip('"')) / 100;
  pressure = float(values['data']['pressure'].strip('"')) / 100;
  
  
  with con:
    cur = con.cursor()

    insertString = "INSERT INTO info(temp, pressure, light, humidity)" + \
                   "VALUES('%f','%f','%f','%f')" % (temp, pressure, light, humidity)

    cur.execute(insertString)

  time.sleep(2)

arduino.close()

