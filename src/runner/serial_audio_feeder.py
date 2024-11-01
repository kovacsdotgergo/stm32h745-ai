import wave
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


# with wave.open("output.wav", "wb") as wf, serial.Serial(
#     "COM3", baudrate=460800, timeout=1
# ) as ser:
with serial.Serial("COM5", baudrate=460800, timeout=1) as ser:
    stop = threading.Event()
    reader = threading.Thread(target=reader_func, args=(ser,))
    reader.start()
    # TODO: add logging about timing

    p = pyaudio.PyAudio()
    # wf.setnchannels(CHANNELS)
    # wf.setsampwidth(p.get_sample_size(FORMAT))
    # wf.setframerate(RATE)

    stream = p.open(format=FORMAT, channels=CHANNELS, rate=RATE, input=True)

    print("Recording...")
    for _ in range(0, RATE // CHUNK * RECORD_SECONDS):
        frame = stream.read(CHUNK)
        # wf.writeframes(frame)
        ser.write(frame)

    print("Done")

    stream.close()
    p.terminate()

    stop.set()
    reader.join()
