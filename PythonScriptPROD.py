import time
import os
from datetime import datetime
from ruuvitag_sensor.ruuvitag import RuuviTag
import sqlite3
from sqlite3 import Error
#sss
# Change here your own device's mac-address
mac = 'F8:0C:D7:84:57:F6'
mac2 = 'FA:67:DF:BE:4C:F9'

def create_connection(db_file):
    conn = None
    try:
        conn = sqlite3.connect(db_file)
    except Error as e:
        print(e)

    return conn

def create_measurement(conn, measurement):
    sql1 = ' INSERT INTO ruuvi1 VALUES(?, ?, ?, ?); '
    cur = conn.cursor()
    cur.execute(sql1, measurement)
    conn.commit()
    return cur.lastrowid


def create_measurementPro(conn, measurement):
    sql1 = ' INSERT INTO ruuviPro VALUES(?, ?, ?, ?);'
    cur = conn.cursor()
    cur.execute(sql1, measurement)
    conn.commit()
    return cur.lastrowid

print('Starting')

sensor = RuuviTag(mac)
sensor2 = RuuviTag(mac2)

conn=create_connection('ruuvidata.sqlite')

while True:

    data = sensor.update()
    data2 = sensor2.update()

    line_sen = str.format('Sensor - {0}', mac)
    line_tem = str.format('Temperature: {0} C', data['temperature'])
    line_hum = str.format('Humidity:    {0}', data['humidity'])
    line_pre = str.format('Pressure:    {0}', data['pressure'])

    line_sen2 = str.format('Sensor - {0}', mac2)
    line_tem2 = str.format('Temperature: {0} C', data2['temperature'])
    line_hum2 = str.format('Humidity:    {0}', data2['humidity'])
    line_pre2 = str.format('Pressure:    {0}', data2['pressure'])


    with conn:
        msr=(mac, data['temperature'], data['humidity'], str(datetime.now().strftime("%d-%m-%Y %H:%M:%S")))
        msr2=(mac2, data2['temperature'], data2['humidity'], str(datetime.now().strftime("%d-%m-%Y %H:%M:%S")))
        
        create_measurement(conn, msr)
        create_measurementPro(conn, msr2)

    # Wait for 9 seconds and start over again
    try:
        time.sleep(1)

    except KeyboardInterrupt:
        # When Ctrl+C is pressed execution of the while loop is stopped
        print('Exit')
        break