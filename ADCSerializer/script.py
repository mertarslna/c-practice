import struct
import serial
import socket

from pyulog import ULog

ser = serial.Serial("/dev/ttyACM0", 115200)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
    size = struct.unpack(">I", ser.read(4))[0]     # Read 4-byte big-endian size
    data = ser.read(size)                          # Read the full payload
    sock.sendto(data, ("127.0.0.1", 49000))         # Forward to PlotJuggler