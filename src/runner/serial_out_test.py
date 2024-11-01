import serial
import time
import threading

# Set up the serial connection (adjust 'COM3' and 'baudrate' as needed)
runtime = 10
frequency = 16000

def precise_periodic_task(frequency, ser):
    interval = 1 / frequency
    repeat = int(runtime / interval)

    i = 0
    counter = 0
    while i < repeat:
        ser.write(b'0x00' + b'0x00')
        # value_bytes = counter.to_bytes(2, byteorder='little')
        # ser.write(value_bytes)
        # counter = (counter + 1) % (2**16)
        # time.sleep(interval / 10)
        i += 1

def reader_func(ser):
    global stop
    while True:
        print(ser.readline().decode('ascii'), end='')

        if stop.is_set():
            return

if __name__ == "__main__":
    with serial.Serial('/dev/ttyACM0', baudrate=460800, timeout=1) as ser:
        stop = threading.Event()
        reader = threading.Thread(target=reader_func, args=(ser,))
        reader.start()

        burst = frequency
        range_beg = 0
        for i in range(runtime):
            begin_run = time.perf_counter_ns()

            range_end = range_beg + burst
            array = b''.join([(x % (2**16)).to_bytes(2, 'little') for x in range(range_beg, range_end)])
            # with open(f"python_tmp{i}", "wb") as file:
            #     file.write(array)
            range_beg = range_end

            begin = time.perf_counter_ns()
            ser.write(array)
            end = time.perf_counter_ns()

            send_time = (end - begin) / 1e9
            print("TIME to send data: ", send_time)

            begin_sleep = time.perf_counter_ns()
            sleep_time = (begin_sleep - begin_run) / 1e9
            period = 1
            if sleep_time < period:
                time.sleep(period - sleep_time)
            else:
                print("Didn't sleep: ", sleep_time)
        stop.set()
        reader.join()
