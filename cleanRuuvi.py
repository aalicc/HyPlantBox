import time
import os
from datetime import datetime
import sqlite3
from sqlite3 import Error

def create_connection(db_file):
    conn = None
    try:
        conn = sqlite3.connect(db_file)
    except Error as e:
        print(e)

    return conn

def emptyTable1 (conn):
    sql1 = ' DELETE from ruuvi1; '
    cur = conn.cursor()
    cur.execute(sql1)
    conn.commit()
    return cur.lastrowid

def emptyTable2 (conn):
    sql1 = ' DELETE from ruuviPro; '
    cur = conn.cursor()
    cur.execute(sql1)
    conn.commit()
    return cur.lastrowid

conn=create_connection('ruuvidata.sqlite')

print('Starting cleanup in 5 minutes')

while True:
    with conn:
        emptyTable1(conn)
        emptyTable2(conn)
    
    try:
        time.sleep(300)

    except KeyboardInterrupt:
        print('Exit')
        break
