import socket
import smbus
import time

# Define Arduino I2C address
arduino_address = 0x04

# Create a SMBus object for I2C communication
bus = smbus.SMBus(1)  # Use bus number 1 for /dev/i2c-1

# Configure UDP server
UDP_IP = "0.0.0.0"  # Listen on all interfaces
UDP_PORT = 5005
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print("UDP server is running...")

def send_command(command):
    try:
        bus.write_byte(arduino_address, ord(command))  # Send command to Arduino
    except IOError as e:
        print("I2C communication error:", e)


# Listen for incoming data
try:
    while True:
        data, addr = sock.recvfrom(1024)  # buffer size is 1024 bytes

        # Process received command
        if data.decode() == 'right':
            send_command('r')
        elif data.decode() == 'left':
            send_command('l')
        elif data.decode() == 'between':
            send_command('c')
        elif data.decode() == 'NO':
            send_command('n')

except KeyboardInterrupt:
    print("Exiting...")
finally:
    sock.close()  # Close the socket when exiting