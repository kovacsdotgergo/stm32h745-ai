# %%
import time, pickle
import numpy as np

# to be able to import the keyword_spotting code
import os, sys

script_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(script_dir, "keyword_spotting"))
import get_dataset as gd
from argparse import Namespace

runtime = 1
frequency = 15872


def get_flags():
    # default command line parameters from keyword_spotting.kws_util
    flags = Namespace(
        data_dir="/home/gergo/data",
        bg_path="/home/gergo/workspace",
        background_volume=0.1,
        background_frequency=0.8,
        silence_percentage=10.0,
        unknown_percentage=10.0,
        time_shift_ms=100.0,
        sample_rate=16000,
        clip_duration_ms=1000,
        window_size_ms=30.0,
        window_stride_ms=20.0,
        feature_type="mfcc",
        dct_coefficient_count=10,
        epochs=36,
        num_train_samples=-1,
        num_val_samples=-1,
        num_test_samples=-1,
        batch_size=100,
        num_bin_files=1000,
        bin_file_path="/home/gergo/kws_test_files",
        model_architecture="ds_cnn",
        run_test_set=True,
        saved_model_path="trained_models/kws_model.h5",
        model_init_path=None,
        tfl_file_name="trained_models/kws_model.tflite",
        learning_rate=1e-05,
        lr_sched_name="step_function",
        plot_dir="./plots",
        target_set="test",
    )
    return flags


# getting the waverform and transforming using cmsis
flags = get_flags()
flags.data_dir = f"{script_dir}/dataset"
flags.batch_size = 1
ds_wave_train, ds_wave_test, ds_wave_val = gd.get_training_data(flags, get_waves=True)

print(len(ds_wave_test))
data_to_export = ds_wave_train.take(10).unbatch().batch(1).as_numpy_iterator()

target_dir = os.path.join(script_dir, "runner_inputs")
os.makedirs(target_dir, exist_ok=True)

for i, (wave, wave_label) in enumerate(data_to_export):
    with open(os.path.join(target_dir, f"test_file_{i}.pkl"), "wb") as file:
        pickle.dump((wave, wave_label), file)
    print("first few", wave[0, 0], wave[0, 1], wave[0, 2])

# begin_run = time.perf_counter_ns()
# end_run = time.perf_counter_ns()
# print((end_run - begin_run)/ 1e9)
