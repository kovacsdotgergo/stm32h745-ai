import threading, time, serial, pickle, os
import numpy as np
# to be able to import the keyword_spotting code

script_dir = os.path.dirname(__file__)
time_pretext = "[time]"

RUNTIME = 10
WAVE_BLOCK_LEN = 15872
N_LABELS = 12

test_files_dir = os.path.join(script_dir, os.pardir, "nn", "runner_inputs")

def reader_func(ser):
    global stop
    while True:
        print(ser.readline().decode('ascii'), end='')

        if stop.is_set():
            return

if __name__ == "__main__":
    '''The input files can be generated with src/nn/export_for_test_runner.py'''
    with serial.Serial('/dev/ttyACM0', baudrate=460800, timeout=1) as ser:
        stop = threading.Event()
        reader = threading.Thread(target=reader_func, args=(ser,))
        reader.start()

        burst = WAVE_BLOCK_LEN
        range_beg = 0
        for i in range(RUNTIME):
            begin_run = time.perf_counter_ns()

            file_name = f"test_file_{i}.pkl"
            with open(os.path.join(test_files_dir, file_name), "rb") as file:
                wave, wave_label =pickle.load(file)
                wave = wave[:, :WAVE_BLOCK_LEN]

            array = wave.tobytes()

            begin = time.perf_counter_ns()
            ser.write(array)
            end = time.perf_counter_ns()

            send_time = (end - begin) / 1e9
            print(time_pretext, "TIME to send data: ", send_time)
            ground_truth = np.zeros(N_LABELS)
            ground_truth[wave_label] = 1
            print("GT:")
            print(" ".join(f"{num:.2f}" for num in ground_truth))

            begin_sleep = time.perf_counter_ns()
            sleep_time = (begin_sleep - begin_run) / 1e9
            period = 1
            if sleep_time < period:
                time.sleep(period - sleep_time)
                print(time_pretext, "Slept: ", period - sleep_time)
            else:
                print(time_pretext, "Didn't sleep: ", sleep_time)
        stop.set()
        reader.join()
