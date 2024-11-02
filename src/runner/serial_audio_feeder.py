import wave
import time
import threading
import serial

import pyaudio

CHUNK = 512
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 16000
RECORD_SECONDS = 60


def reader_func(ser):
    global stop
    while True:
        print(ser.readline().decode("ascii"), end="")

        if stop.is_set():
            return


with serial.Serial("COM5", baudrate=460800, timeout=1) as ser:
    stop = threading.Event()
    reader = threading.Thread(target=reader_func, args=(ser,))
    reader.start()
    # TODO: add logging about timing

    p = pyaudio.PyAudio()

    stream = p.open(format=FORMAT, channels=CHANNELS, rate=RATE, input=True)

    print("Recording...")
    for _ in range(0, RATE // CHUNK * RECORD_SECONDS):
        beg = time.perf_counter_ns()
        frame = stream.read(CHUNK)
        ser.write(frame)
        end = time.perf_counter_ns()
        period = (end - beg) / 1e9
        print(f"Period: {period:.4f} (exp: {CHUNK / RATE})")

    print("Done")

    stream.close()
    p.terminate()

    stop.set()
    reader.join()
