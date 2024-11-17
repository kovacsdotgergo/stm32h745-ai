import threading, time, serial, pickle, os, re
import wave
import numpy as np

script_dir = os.path.dirname(__file__)
# time_pretext = "[time]"

RUNTIME = 4
WAVE_BLOCK_LEN = 15872
BUFFER_BLOCK_NUM = 4  # todo set this up, sleep between these blocks
assert WAVE_BLOCK_LEN % BUFFER_BLOCK_NUM == 0
BUFFER_BLOCK_LEN = WAVE_BLOCK_LEN // BUFFER_BLOCK_NUM
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
        print(ser.readline().decode("ascii"), end="")

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
    """Only a few samples are exported"""
    exported_num = 10
    index = index % exported_num
    file_name = f"test_file_{index}.pkl"
    with open(os.path.join(test_files_dir, file_name), "rb") as file:
        waveform, wave_label = pickle.load(file)
        waveform = waveform[:, :WAVE_BLOCK_LEN]
    return waveform, wave_label[0]


if __name__ == "__main__":
    """The input files can be generated with src/nn/export_for_test_runner.py"""
    with serial.Serial("/dev/ttyACM0", baudrate=460800, timeout=1) as ser:
        stop = threading.Event()
        reader = threading.Thread(target=reader_func, args=(ser,))
        reader.start()

        range_beg = 0
        for i in range(RUNTIME):
            begin_run = time.perf_counter()

            waveform, wave_label = get_kws_test_file(i)  # get_own_test_file(i)

            for j in range(BUFFER_BLOCK_NUM):
                begin_write = time.perf_counter()
                array = waveform[
                    :, j * BUFFER_BLOCK_LEN : (j + 1) * BUFFER_BLOCK_LEN
                ].tobytes()
                ser.write(array)
                end_write = time.perf_counter()
                write_time = end_write - begin_write
                period = 1 / BUFFER_BLOCK_NUM
                if write_time < period:
                    time.sleep(period - write_time)
                    print("[INFO] Write time : ", write_time, f" (exp: {period})")
                else:
                    print("[WARN] Didn't sleep: ", write_time, f" (exp: {period})")

            ground_truth = np.zeros(N_LABELS)
            ground_truth[wave_label] = 1
            print("GT:")
            print(
                " ".join(f"{num:.2f}" for num in ground_truth), WORD_LABELS[wave_label]
            )
            end_run = time.perf_counter()
            print("[INFO] Period: ", end_run - begin_run, " (exp: 1)")
            print("---")

        stop.set()
        reader.join()
