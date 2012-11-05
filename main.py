import serial
import MySQLdb as mdb
import sys

con = None

con = mdb.connect('localhost', 'root', 'password', 'serial');



cur = con.cursor()
cur.execute("CREATE TABLE IF NOT EXISTS \
             info(id INT PRIMARY KEY AUTO_INCREMENT, \
             temp VARCHAR(25), \
             time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP)")
