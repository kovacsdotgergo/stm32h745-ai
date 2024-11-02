import threading, time, serial, pickle, os, re
import wave
import numpy as np
# to be able to import the keyword_spotting code

script_dir = os.path.dirname(__file__)
# time_pretext = "[time]"

RUNTIME = 60
WAVE_BLOCK_LEN = 15872
N_LABELS = 12
WORD_LABELS = [
    "Down",
    "Go",
    "Left",
    "No",
    "Off",
    "On",
    "Right",
    "Stop",
    "Up",
    "Yes",
    "Silence",
    "Unknown",
]

test_files_dir = os.path.join(script_dir, os.pardir, "nn", "runner_inputs")

def reader_func(ser):
    global stop
    while True:
        print(ser.readline().decode('ascii'), end='')

        if stop.is_set():
            return

def get_own_test_file(index):
    record_len = 15872
    test_dir = os.path.join(script_dir, os.pardir, "nn", "test_samples")
    index = index % len(os.listdir(test_dir))
    # Pattern to match files in the form test_file_{index}_{label}.wav
    pattern = rf"test_file_{index}_(.+)\.wav"

    # Search for the file with the specified index
    for f in os.listdir(test_dir):
        match = re.match(pattern, f)
        if match:
            filename = f
            label_name = match.group(1)
            label = [i for i, elem in enumerate(WORD_LABELS) if elem == label_name][0]

    ifile = wave.open(os.path.join(test_dir, filename))
    samples = ifile.getnframes()
    audio = ifile.readframes(samples)

    array = np.frombuffer(audio, dtype=np.int16).reshape(1, WAVE_BLOCK_LEN)
    return array, label

def get_kws_test_file(index):
    '''Only a few samples are exported'''
    exported_num = 10
    index = index % exported_num
    file_name = f"test_file_{index}.pkl"
    with open(os.path.join(test_files_dir, file_name), "rb") as file:
        waveform, wave_label =pickle.load(file)
        waveform = waveform[:, :WAVE_BLOCK_LEN]
    return waveform, wave_label

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

            waveform, wave_label = get_own_test_file(i)

            array = waveform.tobytes()

            begin = time.perf_counter_ns()
            ser.write(array)
            end = time.perf_counter_ns()

            send_time = (end - begin) / 1e9
            # print(time_pretext, "TIME to send data: ", send_time)
            ground_truth = np.zeros(N_LABELS)
            ground_truth[wave_label] = 1
            print("---")
            print("GT:")
            print(
                " ".join(f"{num:.2f}" for num in ground_truth), WORD_LABELS[wave_label]
            )

            begin_sleep = time.perf_counter_ns()
            sleep_time = (begin_sleep - begin_run) / 1e9
            period = 1
            if sleep_time < period:
                time.sleep(period - sleep_time)
                # print(time_pretext, "Slept: ", period - sleep_time)
            # else:
            # print(time_pretext, "Didn't sleep: ", sleep_time)
        stop.set()
        reader.join()
