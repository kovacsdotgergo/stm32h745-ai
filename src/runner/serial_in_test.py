import serial
import time
import ctypes

ser = serial.Serial('/dev/serial/by-id/usb-STMicroelectronics_STLINK-V3_0031004F3137511039383538-if02', baudrate=460800, timeout=0.01)
runtime_in_sec = 180

def read_serial_16():
    ser.reset_input_buffer()
    prev = b'0x01'
    while True:
        begin = ser.read(1)
        begin_value = int.from_bytes(prev + begin, "little", signed=False)
        if begin_value == 0:
            break
        prev = begin
    print("STARTING")
    prev_byte = None
    i = 0
    max_in_line = 0
    errs = 0
    while True:
        i += 1
        if i % (runtime_in_sec * 16000) == 0:
            break
        waiting = ser.in_waiting
        if max_in_line < waiting:
            max_in_line = waiting

        current_byte = ser.read(2)
        current_byte_value = int.from_bytes(current_byte, "little", signed=False)

        if prev_byte is not None:
            if current_byte_value != (prev_byte + 1) % (2**16):
                errs += 1
                prev_byte = None
            else:
                prev_byte = current_byte_value

    print(f"Errors: {errs}, max in line: {max_in_line}")

# def read_serial_8():
#     prev_byte = None
#     while True:
#         current_byte = ser.read(1)
#         current_byte_value = int.from_bytes(current_byte, "little", signed=False)

#         if prev_byte is not None:
#             if current_byte_value != (prev_byte + 1) % (2**8):  # check for wraparound at 255
#                 print(f"Bytes out of sequence! Previous: {prev_byte}, Current: {current_byte_value}")

#         prev_byte = current_byte_value

if __name__ == "__main__":
    read_serial_16()
