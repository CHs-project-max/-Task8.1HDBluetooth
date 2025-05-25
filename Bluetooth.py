import asyncio
import struct
import RPi.GPIO as io
import time
from bleak import BleakClient, BleakScanner
CHAR_UUID = "19B10001-E8F2-537E-4F6C-D104768A1214"  

TRIG = 4
ECHO = 11
io.setmode(io.BCM)
io.setup(TRIG, io.OUT)
io.setup(ECHO, io.IN)

def get_distance():
    io.output(TRIG, True)
    time.sleep(0.00001)
    io.output(TRIG, False)
    while io.input(ECHO) == 0:
        pulse_start = time.time()
    while io.input(ECHO) == 1:
        pulse_end = time.time()
    pulse_duration = pulse_end - pulse_start
    distance = pulse_duration * 17150 
    return distance

async def run():
    device = await BleakScanner.find_device_by_name("LED")
    if device is None:
        print("no device found")
        return
    print("device found")
    async with BleakClient(device, timeout = 30) as client:
        print(f"Connected to {ARDUINO_MAC}")
        #get distance and send to arduino
        while client.is_connected:
            distance = get_distance()
            float_bytes = struct.pack('<f', distance)
            print(f"Sending float: {distance}")
            await client.write_gatt_char(CHAR_UUID, float_bytes)
            print("Float sent successfully.")
            time.sleep(1)
        await client.disconnect()

while True:
    try:
        asyncio.run(run())
    except KeyboardInterrupt:
       break
    except:
        print("Waiting for connection...")
        time.sleep(5)
