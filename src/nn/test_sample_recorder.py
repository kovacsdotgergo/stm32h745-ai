import wave
import threading
import serial
import os
import re
import pyaudio

script_dir = os.path.dirname(__file__)
CHUNK = 512
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 16000
RECORD_SECONDS = 1
CURRENT_CLASS_IDX = 3
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


def get_next_index(directory):
    # Pattern to match files in the form test_file_{idx}_{label}.wav
    pattern = r"test_file_(\d+)_.*\.wav"
    max_index = -1
    for filename in os.listdir(directory):
        match = re.match(pattern, filename)
        if match:
            index = int(match.group(1))
            max_index = max(max_index, index)

    return max_index + 1


if __name__ == "__main__":
    test_dir = os.path.join(script_dir, "test_samples")
    os.makedirs(test_dir, exist_ok=True)

    next_idx = get_next_index(test_dir)
    name = f"test_file_{next_idx}_{WORD_LABELS[CURRENT_CLASS_IDX]}.wav"

    filepath = os.path.join(test_dir, name)
    with wave.open(filepath, "wb") as wf:
        p = pyaudio.PyAudio()
        wf.setnchannels(CHANNELS)
        wf.setsampwidth(p.get_sample_size(FORMAT))
        wf.setframerate(RATE)

        stream = p.open(format=FORMAT, channels=CHANNELS, rate=RATE, input=True)

        print(f"Recording for {WORD_LABELS[CURRENT_CLASS_IDX]}...")
        for _ in range(0, RATE // CHUNK * RECORD_SECONDS):
            frame = stream.read(CHUNK)
            wf.writeframes(frame)

        print("Done")

        stream.close()
        p.terminate()

    with wave.open(filepath, "rb") as wf:
        p = pyaudio.PyAudio()
        stream = p.open(
            format=p.get_format_from_width(wf.getsampwidth()),
            channels=wf.getnchannels(),
            rate=wf.getframerate(),
            output=True,
        )
        data = wf.readframes(CHUNK)

        while data:
            stream.write(data)
            data = wf.readframes(CHUNK)

        stream.start_stream()
        stream.close()
        p.terminate()
