# Documentation of progress

## Useful resources

* c cpp properties different configurations for the two core [link](https://code.visualstudio.com/docs/cpp/c-cpp-properties-schema-reference)

* debug usign vscode [link](https://hbfsrobotics.com/blog/configuring-vs-code-arm-development-stm32cubemx) [another blog link](https://wiki.octanis.org/stm32/vscode) [tutorial link](https://www.youtube.com/watch?v=g2Kf6RbdrIs)

* cubeai and tflite tutorial [link](https://www.digikey.com/en/maker/projects/tinyml-getting-started-with-stm32-x-cube-ai/f94e1c8bfc1e4b6291d0f672d780d2c0)

* cortex-debug extension documentation [link](https://github.com/Marus/cortex-debug/wiki#vscode-settings-for-cortex-debug)

## Building and debugging

The ST developer tools are often available with cli interface and can be integrated with text editor instead of cube ide. Getting familiar with this porvides useful insight into the required programs and their details. Also modern environments usually offer greater costumizibility and useful features that are not present in eclipse.

### Code generation

CubeMX is available as a standalone program and also integrated inside CubeIDE the eclipse developement environment from ST. By default it generates eclipse projects, but newer versions support project generation where a single build makefile and the source code will be generated.

Generatign and using a makefile project lets you see how the building of the project is performed without writing a makefile from scratch.

Usual problems inside the makefile when generatign for stm32h745 are mixing the options of the two cores, also mixing up the files between the cores. Some paths were set up incorrectly but these can be easily spotted.

I decided to version control with git. I have a generated project that is not modified after generation, but the required code addtions can be seen for each peripheral and core. The changes are integrated anohter project which is not managed by the code generator. This way user code is newer overwritten, generated code can be changed to remove errors.

The code generator also updates the generated drivers each time, so by default no library update can be performed. The generator only allows a few old versions for example for FreeRTOS as well.

The generated project also didn't take the relation between the standard C library and FreeRTOS into account that required different settings and implementation for functions supporting the standard library (newlib).

* todo: longer about the newlib problem
  * sys timers
  * hal calilng mallock
  * standard lib calling mallocs
  * reentrancy option in config, support functions required

### VSCode support for embedded C projects

VSCode is by default only a text editor but it support debugging with help of community written extension, code completion, build and other tasks to automate.

Code completions, highlighting, errors are provided by VSCode if the include paths, defines, build options are set up and provided in a config file. For a multicore project or for debug and release builds different configurations can be used. Text editing is faster and more intelligent than in eclipse.

Tasks can be set up to provide shourtcuts to launching programs or run scripts. I set up tasks for building, probing the board, and flashing. More options can be added to launch a program monitoring serial communication.

To debug embedded applications via a debugger there is an extension which provides a graphical interface for gdb. The usual debug operations can be performed. For a multicore project VSCode is capable to manage simultaneously more debug sessions.

Developing inside developer containers is also possible to have consistent envirionment.

### Debugging

I have tried debugging using openocd and the stlink server.

todo: more about these debug servers

After launcing a debug server the code can be downloaded and the cores can be reset by a reset of choice. The debug server listens on several ports in case of openocd, where debuggers can connect. The gdb server can be used after connection via the command line or via some ohter graphical interface.

### Flashing

There are tools from different producers that can flash the board. ST has it's own tools, that are the most capable, I used this to erase to board when the onboard stlink can't see/access the microcontroller. It can connect under reset and after powering down the board, then clearing the memory can be performed after any state.

The texane stlink is a simpler tool with less feautres, but I found it easier to use.  `st-info` can be used to check the state of the board. `st-flash` sometimes fails to verify the downloaded code, but after successful debuggier connections it usually works.

The debug servers can use these flashing tools, or others to download code, this can be done when starting a debugging session. Disconnecting after is also an alternative when debug is not required to flash.

## Configurig FreeRTOS

Generating the project using the code provided by the manufacturer is not always sufficient. The two problems that arose were to limited choice of the OS versions and the not proper integration of FreeRTOS [link](https://nadler.com/embedded/newlibAndFreeRTOS.html).

The use of FreeRTOS with newlib or newlib nano requires special care. To make the library work there are a few funcitons that have to be implemented. A few to mention are sbrk that is neccessary for malloc and free, it allocates memory for the allocation, and getpid, exit... Most of the funcitons are basic and are implemented in `syscalls.c`. [link to required funcitons (under doc)](https://sourceware.org/newlib/).

When working in a multitasking environment, reentrancy requires attention. Malloc is an example, where multiple concurrent calls without protection can cause the memory pool to corrupt. Malloc is called in several common library functions.

The default sbrk implementation doesn't take this into account, so no funcions could be safely used requiring malloc. The mentioned source provides a solution where the FreeRTOS tasks are stopped while using malloc. With this solution there is no need for the `heap4.c` files provided by the OS, because the standard funcitons can be utilized (but it might funcion with less guarantees?).

Another problem with the generated ST code is that some HAL functions also use malloc (e.g. the USB driver). When this causes problems, the solution above also solves this problem by disabling interrupts? when mallocing. For this the used stack size is also required (to exclude from possible heap area). This can be guessed, but the solutions also adds a function that returns the used stack size by the USB stack in the interrupt?. (TODO: this part is a bit a foggy for me but probabily not so important, especially without USB)

For the multicore communication a newer version would be nice. To ease the use of Message Buffers, not only the ovewriting of the SEND_COMPLETED? macro is the only solution anymore, for each message buffer seperate callback functions can be added.

## Research

These chapters are in the latex documentation.

## Optimization techniques for NNs

todo: Quantization is somewhat described here in the cubeai documentation under 'Quantized models support'. this will be important for the optimization part of my task

### Microcontrollers

### Instruction sets

### Neural net accelerators

## Neural network runtimes

### tflite

#### Getting started

The documentation on the [main tensorflow site](https://www.tensorflow.org/lite/microcontrollers/get_started_low_level) is not up-to-date, neither is the [article](https://www.digikey.com/en/maker/projects/tinyml-getting-started-with-tensorflow-lite-for-microcontrollers/c0cdd850f5004b098d263400aa294023) I used to get started. Building the examples is now done with a different script and the paths to the headers, source files are also different. The [current documentation](https://github.com/tensorflow/tflite-micro/blob/main/tensorflow/lite/micro/docs/new_platform_support.md) is up-to-date in the migrated repository.

#### Memory consumption

It's important to distinguish, how much change the cpp runtime and how much the framework is.

#### Building tflite

One option is the tree generation script. There is also an example makefile that helps setting up the required flags and sources. When building into a library with my own makefile based on this, or using the example directly, I had to decrease the MicroProfilers array size to fit into the stack. Even after this, the output of the network is not correct either way. The command to create the tree is: `python3 tensorflow/lite/micro/tools/project_generation/create_tflm_tree.py -e hello_world --makefile_options="TARGET=cortex_m_generic OPTIMIZED_KERNEL_DIR=cmsis_nn TARGET_ARCH=cortex-m7+fp" /tmp/tflm-tree`. After this the example makefile within the same folder as this script is copied over. Then some more files are required for the arm register definitions (`tensorflow/lite/micro/tools/make/downloads/cmsis/Device/ARM/ARMCM7/Include/system_ARMCM7.h` and `tensorflow/lite/micro/tools/make/downloads/cmsis/Device/ARM/ARMCM7/Include/ARMCM7_DP.h` inside `tflite-micro`).

There is also a makefile that builds the library directly with the command `make -f tensorflow/lite/micro/tools/make/Makefile TARGET=cortex_m_generic TARGET_ARCH=cortex-m4+fp OPTIMIZED_KERNEL_DIR=cmsis_nn microlite`. To fit in the stack, the profiler has to be adjusted similarly.

CubeMX can also use tflite as a backend when using .tflite networks. This adds a c api for tflite as well. When selecting this, it was not buildable, sources were not inlcuded in the build and absolutely no cpp building process was included for tflite micro.

#### TFLM retrurning wrong outputs after running the net

My first suspition was that the Cpp build system is not correct, so I made a class checking if all static and non static (and const) variables are correctly initialized. All of them are correct, so this should not be a probelm. I also tried with a class where these objects are part of and array after reading about runtime functions required for [threadsafe statics and vector initialization](https://itanium-cxx-abi.github.io/cxx-abi/abi.html#once-ctor). `-fno-threadsafe-statics` controls the thread safety of these varibales, but doesn't turn off the guard variables that are used to check if the static member variable of a class is initialized, this is still required to only have a single init.

I have also checked if there are sections in the cc object files that are not handled (saw .ctros and .dtors sections on forums), but found none. There is one interesting function in the `freertos.o`, a function for static initialization and destruction.

Next is using a single neuron with linear activation to debug. For this I have written a new notebook, in which I used `xxd -i` to produce the binary array.

There are different tools for tflm developement in the repository. For the debug purposes I used two of them:

* The first was `generate_micro_mutable_op_resolver_from_model`. This generates a function that produces the OpResolver class based on a tflite file. It contains the needed layers that are used by the network.
* The second is the `visualize` script that generates html from a tflite file, similar information can be extracted as when using the Tensorflow Lite interpreter in python.

These were all correct and the quantized model required only the fully connected input and still produced the wrong output. The float model with only one layer and one neuron returned the correct output.

##### Debugging the quantized model

The inputs and outputs are not in the correct format. Both are given with float test values and the quantized int8 values are calculated using the scale and zero_point values. These paramterers are not the correct values and when requesting the input and output of the network, even the type of the tensor is incorrect (float32 instead of int8).

The main reason of the incorrect behaviour is that the tensor (`TfLiteTensor`) structure contained wrong quantization parameter and type values (`params` and `type` field). When debugging I suspected that reading in the Flatbuffer format is incorrect. I debugged this part and the format was correct in the functions inside the library (`interpreter.AllocateTensors()` call). The incorrect values appeared when returning from this call. When checking the layout of this type in the debugger it differed inside the library and inside my code, so the typedef had to be incorrect of coming from different sources.

![image from debugger checking the two different layouts of the `TfLiteTensor` struct](images/tflm_debug.PNG)

The two typedefs of this file in `common.h` are guarded by a `TF_LITE_STATIC_MEMORY` define. I have found one [question](https://github.com/tensorflow/tflite-micro/issues/2528) about the function of this define.

#### Binary sizes

Right after C++ build, without even `freertos.cc`.

```shell
text data bss dec hex filename
37168 500 9968 47636 ba14 /home/gergo/workspace/stm32h745-ai/src/h745/Makefile/CM4/build/stm32h745-ai_CM4.elf
```

After adding the tflite networks and tflite:

```shell
text data bss dec hex filename
172752 12524 9984 195260 2fabc /home/gergo/workspace/stm32h745-ai/src/h745/Makefile/CM4/build/stm32h745-ai_CM4.elf
```

Runtime [us]:

|    | optim | no quant | fallback quant | dynamic quant | full quant |
| ---|    ---|       ---|             ---|            ---|         ---|
|cm4 | -O0   | 155      | 250            | 155           | 225        |
|cm4 | -O3   | 28       | 38             | 28            | 32         |
|cm4 | -Os   | 34       | 42             | 34            | 35         |
|cm4 | -Ofast| 29       | 39             | 29            | 32         |
|cm7 | -O0   | 50       | 87             | 50            | 77         |
|cm7 | -O3   | 10       | 10             | 10            | 9          |
|cm7 | -Os   | 13       | 13             | 13            | 11         |
|cm7 | -Ofast| 10       | 11             | 10            | 9          |

Storage:

|     | optim | text   | data   | bss    | dec    | hex     | note      |
|  ---|    ---|     ---|     ---|     ---|     ---|      ---|        ---|
| cm4 | -O0   | 37160  | 500    | 9968   | 47628  | ba0c    | C base*   |
| cm4 | -O3   | 32488  | 500    | 9968   | 42956  | a7cc    | C base*   |
| cm4 | -O0   | 37268  | 500    | 9968   | 47636  | ba14    | no tflite |
| cm4 | -O0   | 172752 | 12524  | 9984   | 195260 | 2fabc   |           |
| cm4 | -O3   | 88724  | 12520  | 9976   | 111220 | 1b274   |           |
| cm4 | -Os   | 76460  | 12520  | 9976   | 98956  | 1828c   |           |
| cm4 | -Ofast| 88692  | 12520  | 9976   | 111188 | 1b254   |           |
| cm7 | -O0   | 55580  | 500    | 11256  | 67336  | 10708   | C base*   |
| cm7 | -O3   | 44620  | 500    | 11256  | 56376  | dc38    | C base*   |
| cm7 | -O3   | 97808  | 12520  | 11272  | 121600 | 1db00   |           |
| cm7 | -Os   | 82340  | 12520  | 11264  | 106124 | 19e8c   |           |
| cm7 | -Ofast| 97688  | 12520  | 11272  | 121480 | 1da88   |           |

(*) C base is from the feat/nn_frameworks branch, the C base code used as the starting point
todo

#### CubeAI

After adding the network in MX, there are different options to generate applications. By default only the header and source files interfacing the library are generated for the imported network. The options to add more functionality by the code generator are: performance measurement application, template application and [todo, validation?].

The performance measurement application is quite complex, it communicates the measurement results via the serial port. The source code is not easily comprehendable.

Easier options are the blog post about cubeai and tflite, but it was written for a previous version of these frameworks, so the code had to be modified. Similar example code is also available in the documentation of the library. The generated template application is only a touch more complex, so it can also be easily modified.

Building the application on M7 with cubeai example net:
Release:
```shell
 text    data     bss     dec     hex filename
57228    2104   11528   70860   114cc build/stm32h745-ai_CM7.elf
```
runtime ~ 7.5 us

Debug:
```shell
 text    data     bss     dec     hex filename
68784    2184   11528   82496   14240 build/stm32h745-ai_CM7.elf
```
runtime ~ 7.8 us

for m4:
Release:
```shell
 text    data     bss     dec     hex filename
42564    2104   10280   54948    d6a4 build/stm32h745-ai_CM4.elf
```
runtime ~ 30.17 us

Debug:
```shell
 text    data     bss     dec     hex filename
48152    2184   10280   60616    ecc8 build/stm32h745-ai_CM4.elf
```
runtime ~ 30.7 us

the base project:
Debug:
```shell
 text    data     bss     dec     hex filename
55580     500   11256   67336   10708 build/stm32h745-ai_CM7.elf
 text    data     bss     dec     hex filename
37160     500    9968   47628    ba0c build/stm32h745-ai_CM4.elf
```

Release:
```shell
 text    data     bss     dec     hex filename
44620     500   11256   56376    dc38 build/stm32h745-ai_CM7.elf
 text    data     bss     dec     hex filename
32488     500    9968   42956    a7cc build/stm32h745-ai_CM4.elf
```

### Merging the two frameworks

todo

## Searching a nerual network

The two options are object detection in image processing, and keyword spotting/detection (or maybe this can be called object detection on audio signals as well). The memory on both cores is 1M, so the final net has to be tiny to fit.

For image processing some yolo micro or yolo-lite is an option, but these are larger models, the smallest has around 2-3M parameters.

Mlperf tiny nets have to be adequate.

### openWakeWord

In the [github repository](https://github.com/dscripka/openWakeWord) they mention, that it is probably not suitable for embedded devices. There is a [modified repo](https://github.com/kahrendt/microWakeWord) that can run on microcontrollers.

### Apples article about their earlier detection net

[The article](https://machinelearning.apple.com/research/hey-siri) described the network and algorithm used.

### Porcupine

[Porcupine](https://picovoice.ai/platform/porcupine/) is a commercial wakeword detection engine. It can run on M7 and M4. New keywords can be set in text and the trained model can be downloaded. It is free for use without support and with a limited number of customers. On a raspberry pi it supposedly only required a few percent of the CPU runtime.

### Fluent.ai

Another commercial ready-made [solution](https://fluent.ai/contact/), that offers the detection for constrained devices. There is no documentation, and can only be used after contacting the company.

### EfficientWord-net

Open source project ([article](https://medium.com/ant-brain/efficientword-net-an-open-source-hotword-detector-50058d68149f), [repo and documentation with paper link](https://github.com/Ant-Brain/EfficientWord-Net)). Currently they promise an 88MB model, that runs on rasperry pi.

### Tiny ML wakeword detection

Open [source project](https://www.hackster.io/team-wakeup/tiny-ml-wake-word-detection-964278), that uses a tensroflow example training script. It runs on arduino. This might just be the plain TFLM example project.

### Snowboy

Engine, but raspberry pi is at [least required](https://github.com/Kitt-AI/snowboy).

### OpenAi whisper models

Transformer based speech translation model. There are different [models](https://huggingface.co/openai/whisper-tiny.en), unfortunately the tiny is 88MB large.

### Pyannote speaker segmentation

The [paper](https://arxiv.org/abs/2104.04045) mentions that their net has 1.5M parameters. The source [code](https://huggingface.co/pyannote/segmentation) is available as a part of the reproducible experiment. There is a seperate [model?](https://huggingface.co/pyannote/voice-activity-detection) for speaker activity detection.

### WaveNet

Dilated convolutional nns started for 1D with WaveNet. There is a [blog post](https://medium.com/@kion.kim/wavenet-a-network-good-to-know-7caaae735435) about the details of implementation. Otherwise PixelCNN might be the first net applying these ideas. These are unfortunately generative networks, so can't be directly applied here.

### The MLPerf Tiny benchmark nets

[The rules in the repo are about the implementation of the benchmarks](https://github.com/mlcommons/tiny/blob/master/benchmark/MLPerfTiny_Rules.adoc)([original page of the benchmark](https://mlcommons.org/benchmarks/inference-tiny/)). They use the voice commands dataset. The python files for the model and the dataset are avalable in the repo. The model is a DS-CNN.

#### Silabs

Silabs have their own [solution](https://github.com/SiliconLabs/mltk/blob/master/mltk/models/tinyml/keyword_spotting.py) based on the MlPerf code. On their [webpage](https://siliconlabs.github.io/mltk/docs/python_api/models/index.html) several models for microcontorllers are listed.

#### ARM

This led me the an ARM [repo](https://github.com/ARM-software/ML-KWS-for-MCU) for KWS. The models are specifically made for MCUs and come in different sizes.

The KWS nets are nicely summarized in [Hello Edge: Keyword Spotting on Microcontrollers](/mnt/e/BME/VIK/MSc/Onallo_labor/dipterv1/sources/ai/arm_hello_edge.pdf). [For deployment](https://github.com/ARM-software/ML-KWS-for-MCU/blob/master/train.py) they used the CMSIS-NN library directly.

#### Google

The google KWS [repo](https://github.com/google-research/google-research/blob/master/kws_streaming/README.md) contains some more models, including the previous [paper](/mnt/e/BME/VIK/MSc/Onallo_labor/dipterv1/sources/ai/streaming_keyword_spotting.pdf). The citations can be useful for the better performing options. Also a TFLM [question](https://groups.google.com/a/tensorflow.org/g/micro/c/EidfTbxqk3o?pli=1) about the streaming model leads to this article.

### Tiny CRNN

[Paper](/mnt/e/BME/VIK/MSc/Onallo_labor/dipterv1/sources/ai/tiny-crnn.pdf) about a convolutional GRU with additional attention. It also mentions the streaming implementation of the recurrent part. The models contain from 50K to 2M parameters.

## Setting up the environment for NN

__Benchmark__ files: the keyword spotting tiny benchmarks source code.

I had to update the GPU driver to have access to the recent solutions for running ai workloads. Docker and alternatives now [support](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/latest/install-guide.html) the GPUs pretty well. After following this tutorial to set up the gpu, I have added the start command for the docker daemon (service) in wsl.

The tensorflow containers have most of the required packages already installed. The most recent version did not work for me, luckily I found an older version, that does.

I set up a devcontainer to run the ai scripts inside. The source code is bind mounted and the kws dataset from the tiny benchmark is also downloaded here.

The speech_commands dataset has input samples as a waveform. The benchmark files pad them probably to 16k samples, which is 1s with the 16kHz sampling rate. There is a [paper](/mnt/e/BME/VIK/MSc/Onallo_labor/dipterv1/sources/ai/speech_commands.pdf) about the dataset. The paper states, that some of the files can be shorter than one second, this is why the padding was needed.

### Input preprocessing

The samples can be retrieved in the benchmark code as waves or as preprocessed inputs. Because the models use a spectrogram input, this has to implemented on the device as well.

The `quantization` script performes simple tflite quantization and model export. The preprocessing for test data can be performed using the `make_bin_files` script. It exports the selected inputs to binary after preprocesisng them (the `README` describes these in detail).

## Optimization

todo:
* fusing layers, e.g. fusing batch norm is possbile in torch, as mentioned in [here](https://community.arm.com/arm-community-blogs/b/ai-and-ml-blog/posts/pytorch-to-tensorflow-lite-for-deploying-on-arm-ethos-u55-and-u65)
* using a streaming model can be really efficient for convolutions
* using another model from the sources previously mentioned in the searching part
* cores
* kernel optimization
* quantization, quantization aware training

## KWS benchmark net

The float net had some operations that are not supported in TFLM (different types for the conv2d kernel(int8) and inputs(float32)).

The benchmark net can run with sufficient task stack space. The output of the TFLM profiling for debug:

```text
"Unique Tag","Total ticks across all events with that tag."
CONV_2D, 108195201
DEPTHWISE_CONV_2D, 25946035
AVERAGE_POOL_2D, 277088
RESHAPE, 1384
FULLY_CONNECTED, 56098
SOFTMAX, 32287
"total number of ticks", 134508093

[RecordingMicroAllocator] Arena allocation total 23412 bytes
[RecordingMicroAllocator] Arena allocation head 16004 bytes
[RecordingMicroAllocator] Arena allocation tail 7408 bytes
[RecordingMicroAllocator] 'TfLiteEvalTensor data' used 420 bytes with alignment overhead (requested 420 bytes for 35 allocations)
[RecordingMicroAllocator] 'Persistent TfLiteTensor data' used 64 bytes with alignment overhead (requested 64 bytes for 2 tensors)
[RecordingMicroAllocator] 'Persistent TfLiteTensor quantization data' used 40 bytes with alignment overhead (requested 40 bytes for 4 allocations)
[RecordingMicroAllocator] 'Persistent buffer data' used 5884 bytes with alignment overhead (requested 5812 bytes for 34 allocations)
[RecordingMicroAllocator] 'NodeAndRegistration struct' used 416 bytes with alignment overhead (requested 416 bytes for 13 NodeAndRegistration structs)
```

```shell
arm-none-eabi-size /home/gergo/workspace/stm32h745-ai/src/h745/Makefile/CM7/build/debug/tflm/stm32h745-ai_CM7.elf
text data bss dec hex filename
491384 504 11232 503120 7ad50
```

And for release:

```text
"Unique Tag","Total ticks across all events with that tag."
CONV_2D, 4457435
DEPTHWISE_CONV_2D, 2051249
AVERAGE_POOL_2D, 109619
RESHAPE, 544
FULLY_CONNECTED, 4301
SOFTMAX, 3718
"total number of ticks", 6626866

[RecordingMicroAllocator] Arena allocation total 23412 bytes
[RecordingMicroAllocator] Arena allocation head 16008 bytes
[RecordingMicroAllocator] Arena allocation tail 7404 bytes
[RecordingMicroAllocator] 'TfLiteEvalTensor data' used 420 bytes with alignment overhead (requested 420 bytes for 35 allocations)
[RecordingMicroAllocator] 'Persistent TfLiteTensor data' used 64 bytes with alignment overhead (requested 64 bytes for 2 tensors)
[RecordingMicroAllocator] 'Persistent TfLiteTensor quantization data' used 40 bytes with alignment overhead (requested 40 bytes for 4 allocations)
[RecordingMicroAllocator] 'Persistent buffer data' used 5880 bytes with alignment overhead (requested 5812 bytes for 34 allocations)
[RecordingMicroAllocator] 'NodeAndRegistration struct' used 416 bytes with alignment overhead (requested 416 bytes for 13 NodeAndRegistration structs)
```

```shell
arm-none-eabi-size /home/gergo/workspace/stm32h745-ai/src/h745/Makefile/CM7/build/release/tflm/stm32h745-ai_CM7.elf
text data bss dec hex filename
181636 500 11232 193368 2f358
```

This runtime for the optimized run is 13.8 ms, which is 72 times per second. The input is 16kHz, from which we produce the MFCC. After testing with a few binary samples form the test set, the network produced correct outputs.

__todo__: check how much data is required for one frame (if there is any subsampling step for example for the mel scale), if the net can run at all. (After a quick calculation even without compression, the processing power should be enough).

### Preprocessing

The preprocessing inside `get_dataset.py` in the benchmark repository:
The MFCC implementation in tflite: [link](https://github.com/tensorflow/tensorflow/blob/master/tensorflow/lite/kernels/mfcc.cc).

* it only transforms the data, not the labels
* cast to float32
* reduce max, max scaling
* (pad the back to be `desired_samples` long (1s))
* (scales if the previously specified 1 is not ok (reduce max results in max of 1))
* (shifting the start of the sample, but as I understand, the current setup pads with 2 0's from front and back, then slices starting from the second value, so nothing happens in the end)
* (for training background data is added)

After this the MFCC transformation:

* stft: `tf.signal.stft` with the config variables from the command line
* absolute value of spectrogram
* frequency scale conversion to mel using the default values from [here](https://www.tensorflow.org/api_docs/python/tf/raw_ops/Mfcc)
* log of the apmlitudes to result in logarithmic mel spectrogram

#### CMSIS

The python wrapper of the DSP library calls the C functions. I have first assembled a small python code to test the float implementation. After this other datatypes have to be checked, and the quantization of the inputs in the `make_bin_files` script.

There was a differnece between the calculated MFCC's using CMSIS and the tensorflow preprocessing function. Because in tensorflow the implementation uses STFT, the whole input window is handled at once. The normalization also happens on this 1s segment. Using the cmsis implementation in a loop, there is no normalization (if there was, it would be calculated on one window of the stft, which results in a different max for each window). This caused the difference.

With this normalization the preprocessing can not easily converted to streaming, as for the overlapping parts the normalization is different, so the previous mfcc can not be utilized.

Results from the first check, after running the quantized net on the test sets preprocessed with the different functions.
Accuracy = 0.917 (4482/4890) (original)
Accuracy = 0.918 (4490/4890) (f32 cmsis)
Accuracy = 0.906 (4428/4890) (i32 cmsis)
Accuracy = 0.911 (4453/4890) (i16 cmsis)

There is no significant difference, checking some samples manually, for the higher order mfcc coefficients there is a detectable difference, but for the main components it is really small. The cause of this is still unknown for me.

Also there is a possibility of saturation when using quantized preprocessing. If this happens, and how serious the effect can be, has to be explored. It can probably be assumed that the input is normalized, because the mfcc in only used with the neural net (this might simplify the exploration of the saturation).

##### Difference between the mfcc results

There is a small difference between the mfccs calculated with cmsis, or tensorflow. I have checked the mel scale transformation and the window functions. Both are identical. The intermediate results can be checked by writing a python wrapper around a custom C debug code. This seemed more complicated that I have time for. The other possibility could be to only use the C library and serialize the intermediate results. Then all the stages can be checked. This can also be done on the mcu, because the preprocessing has to be implemented. Then the intermediate results can be saved by using the debugger.

The mfcc may saturate, I checked the max sum of the mel filters, and it can be compared with the mfcc code. It uses several magic numbers and is not well commented, so I couldn't decide if saturation may occur for my configuration.
#### Implementation on the MCU

First the build rules have to be written, then the neccessary files should be listed and copied in this main project. When the C implementation of the preprocessing is done on the device, add log statements for the intermediate results, then compare with the intermediate results of the tensorflow python results.

To build the CMSIS-DSP library, the required sources are listed using a script. This script uses the compilers feature to discover dependencies automatically. If more functions are needed from the library, it can be quickly collected. The size of this repository is smaller this way.

After adding in the preprocessing, malloc failed. There was not enough space, because the current default linker script uses DTCM, which is only 128k. When I increased the required size of the stack and heap in the linker script, it wouldn't fit (at least in debug mode for sure). Other mems should be tested an the speed of the memories compared. Currently with some bigger heap size it still fails, before the scheduler is running, the `sbrk()` function tries to reserve past the current stack pointer. The largest is the D1 RAM on the CM7, I have selected this instead of the DTCM. Later as an optimization step, the DTCM can be used as the fastest mem.

The implementation using max, hereby saturating shows similar results as the the implementation with absmax. The original implementation uses the max solution, but I suspect that in real a real world application in extreme cases the absmax can be more beneficial. Also the absmax seemed to perform slightly better. Results of the evaluation:

|            | test               | val                | train               |
|------------|--------------------|--------------------|---------------------|
| original   | 0.917 (4482/4890)  | 0.956 (9655/10102) | 0.968 (82778/85511) |
| f32        | 0.918 (4490/4890)  | 0.957 (9664/10102) | 0.964 (964/1000)    |
| i32        | 0.906 (4428/4890)  | 0.945 (9550/10102) | 0.951 (951/1000)    |
| i16        | 0.911 (4453/4890)  | 0.948 (9577/10102) | 0.950 (950/1000)    |
| i32_absmax | 0.908 (4438/4890)  | 0.947 (9568/10102) | 0.956 (956/1000)    |
| i16_absmax | 0.915 (4473/4890)  | 0.951 (9603/10102) | 0.956 (956/1000)    |

Due to these resutls I decided to implement the absmax soulution on the device. Also immediately applied all the cmsis fucntions for conversion, scaling, max calculation, etc. (conversion is difficult to implement right without these, also these perform similarly to the naive implementation (for loop)).

M7 release results [ms]:

```shell
Calculate:
f32: 8.027917
q31: 10.955775
q15: 7.854500
Quantize:
using dsp: 0.014533
naive: 0.028433
Run:
load model: 1.304908
setup: 0.869517
junk prints and variables: 224.254135
invoke: 13.580501
junk post print: 11.086708
Full runmodel call: 251.095779
M4 core does nothing...
MAX possible measruement: 17895.697266
```

#### Debugging MFCC

After the stack setup seemed right, I encountered a hard fault while calculating the MFCC result. The hard fault is precise (bus fault), an address outside of the valid memory range is used. It happened because the values inside bss are corrupted. The exact error is inside the task switch, when the current TCB is checked, which pointer value is altered. When debugging inside MFCC, a watchpoint was on this TCB pointer variable. The scaling function uses source and dest pointers, which pointed there. After this the exact root cause when these pointers are corrupted has to be found. The problem was an input buffer with not sufficient size as the input to the mfcc transformation funciton. The overflow currupted the mentioned variables.

Currently I didn't start to check the root cause of the difference between the mfccs. The net performs similarly on these inputs as well.
