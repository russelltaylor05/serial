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
                 temp DECIMAL(4,2) UNSIGNED, \
                 humidity DECIMAL(4,2) UNSIGNED, \
                 pressure DECIMAL(4,2) UNSIGNED, \
                 light DECIMAL(4,2) UNSIGNED, \
                 time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP)")

while True:
    with con:
        cur.execute("INSERT INTO info(temp) VALUES('1.0')")
        print "inserted"
        time.sleep(4)
