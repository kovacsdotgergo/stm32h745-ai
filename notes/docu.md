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

## Feeding the inputs

The possible choices are uart form the debugger or the usb port on the dev board. Using these communication channels I have the option to send test data or use the microphone from the PC. The virtual serial port should be tested if it can handle 16000 Hz 16bit data. This would be the easier choice, as only the debugger is needed and feeding the serial from the PC is also really simple.

The standard required serial baud rate is at least 460800. This might not be possible via UART.

### Input test

The microcontroller is sending two byte values via the serial. A timmer triggers the time when the transmission is required. The data is an increasing value in each step. The PC reads the inputs and checkes if the values are correct. It also prints the maximum length of the input buffer. The result of a 3 min run:

```shell
Errors: 0, max in line: 1527
```

Promising result, no incorrect or missing values.

### Output test

The PC feeds the microcontroller which checks the values. Also using DMA the timing details of the communication should be evaluated.

Reading in a busy loop also gave no errors. The PC sends the data in a block, then initiates the next transaction on every 1s. This is not the same as receiving the data with 16 kHz, but the processing is when the data is collected in a buffer. To test further I am going to implement handling the input data with DMA. Then the content of the buffer is checked.

So the test is to send a large number of test inputs consecutively, and teh controller receives these and checkes the buffer. The transmission happends every 1s for 1s of waveform data, so the abstraction is at the buffer level, handling a full buffer is same in case of a DAC and the test inputs.

#### DMA error debugging

The MCU receives messages using a DMA, which takes the bytes from the UART peripheral. It can use buffering and burst transmission to memory. The first problem was that the received buffer had wrong values randomly (seemingly UART values were dropped), but at every speed from 115200 to 460800.

The root cause was that the D cache wasn't invalidated, which is the most basic problem with cache and DMA. After this the first set (one buffer full) of values were correct, but no further transmissions were successful (no DMA interrupt). The DMA transfer size was registered correctly and the DMA was enabled. During the second set of values the UART input buffer was overrun. This leads to the suspicion that the DMA does not move any data. Breaking after a few sent characters shows that the DMA was in fact idle the whole time, to values were moved into the internal buffer.

This was due to the messed up value of the uart instance (huart3). The base address of the peripheral was changed, so the DMA had an incorrect peripheral address (which could be seen in the SVD view). The huart handle value was changed during the SCB_InvalidateDCache_by_Addr call. This required 32 byte aligned inputs which the wave buffer was not. This caused several additinal variables to be invalidated, that had the correct value in the cache.

After calling the cache invalidation function with the correctly aligned buffer, the wave buffer check passed even for the largest baud rate.

### Testing with FreeRTOS enabled

When the communication showed no errors with the OS disabled, I have assembled a short task that verifies that the behaviour is the same with it enabled. There were no errors. The sample task prints errors when the wavefrom arrived. The DMA interrupt signals with a binary semaphore.

First I am setting up the application to run with the float preprocessing, the quantized version can be measured after this.

## The final application

### Artificial test inputs

The data from the test set is sent in the exact form as it was used during the net evaluation (no offset of the recordings). The result on a few test files is sent back from the device and the ground truth is also printed for comparison. The results are as expected, close to the ground truth.

### The base application

The base application runs on every full buffer. The inputs are collected using DMA and double buffering. When the inputs are ready the processing task is triggered using a binary semaphore. Preprocessing is performed by float mfcc using cmsis. The quantization uses the naive float implmenetation. The net only has a single version. The outputs are processed using argmax and the result is sent back.

The PC feeds the the inputs from the microphone array in blocks and prints the results.

The solution has a larger latency due to sending the data in larger blocks. The UART transmission is similarly fast if using a shorter block size, e.g. 512 instead of 15872. The net only runs once in every second, the keyword are often missed.

With the worse quality microphone integrated into a laptop the accuracy is bad. Swithching to a better quality headset most of the keywords are recognized by the application. Better resolution is neccessary for better localization and therefore accuracy.

### Recording own test files

As the base application recognized few of the keywords while running continuously, I will test with recorded inputs as well. I made several scripts for recording test data, exporting the original files to wav, sending own recording to the device for testing, feeding the device continuously, etc.

### Optimization

Optimization can be runtime, area, sharing between the cores and user experience

#### Improving resolution of the detection

I have added two implementations for this. The first using a circular buffer for reception. From this the waveform is copied to a working buffer. The dma buffer holds one extra block that is used to start the reception as soon as possbile.

The other implementation uses one block less memory and requries on block less copy each cycle. Further differences in performace can be benchmarked. It is documented in the code (`wave_provisioner.c`).

The output is reasonably correct with a few false positives and not too bad accuracy. Further postprocess could be used to correlate with a matched filter that should probably be a triangle signal, and use a threshold on the output of this.

#### Measurements:

```txt
BENCHMARK (min, mean, max)
[irq] start next DMA and stuff: 0.001033, 0.001542, 0.001625
[irq] start next DMA and stuff: 0.001475, 0.001550, 0.001617
[irq] start next DMA and stuff: 0.001500, 0.001563, 0.001650
[irq] start next DMA and stuff: 0.001492, 0.001567, 0.001642
[irq] start next DMA and stuff: 0.001400, 0.001592, 0.001667
[irq] invalidate: 0.003783, 0.003800, 0.003833
[irq] invalidate: 0.003783, 0.003796, 0.003825
[irq] invalidate: 0.003767, 0.003792, 0.003817
[irq] invalidate: 0.003767, 0.003792, 0.003842
[irq] invalidate: 0.003767, 0.003800, 0.003825
[irq] memcpy: 0.010808, 0.011379, 0.012667
[irq] memcpy: 0.010800, 0.011442, 0.013008
[irq] memcpy: 0.010842, 0.011546, 0.012917
[irq] memcpy: 0.010908, 0.011450, 0.012475
[irq] memcpy: 0.010775, 0.011608, 0.012933
[irq] callback: 0.001333, 0.001500, 0.001800
[irq] callback: 0.001233, 0.001475, 0.001808
[irq] callback: 0.001300, 0.001467, 0.001850
[irq] callback: 0.001350, 0.001442, 0.001550
[irq] callback: 0.001325, 0.001429, 0.001492
[task] before preproc: 0.000108, 0.000108, 0.000125
[task] preproc: 7.105766, 7.891912, 8.338908
[task] preproc: 7.869225, 8.165775, 8.336458
[task] preproc: 7.490791, 8.185275, 8.343367
[task] preproc: 7.411391, 8.144046, 8.339275
[task] preproc: 8.104934, 8.241067, 8.344308
[task] quantize: 0.015650, 0.015692, 0.015733
[task] quantize: 0.015625, 0.015679, 0.015733
[task] quantize: 0.015633, 0.015687, 0.015733
[task] quantize: 0.015633, 0.015712, 0.016125
[task] quantize: 0.015633, 0.015712, 0.016100
[task][run] run before invoke: 0.001692, 0.001763, 0.001842
[task][run] run before invoke: 0.001675, 0.001787, 0.002225
[task][run] run before invoke: 0.001675, 0.001787, 0.002092
[task][run] run before invoke: 0.001667, 0.001812, 0.002033
[task][run] run before invoke: 0.001717, 0.001763, 0.001817
[task][run] invoke: 13.570825, 13.574488, 13.578175
[task][run] invoke: 13.568592, 13.575254, 13.580817
[task][run] invoke: 13.568666, 13.572912, 13.577750
[task][run] invoke: 13.567066, 13.574133, 13.579725
[task][run] invoke: 13.567233, 13.573978, 13.578684
[task][run] after invoke: 0.000525, 0.000592, 0.000633
[task][run] after invoke: 0.000517, 0.000558, 0.000633
[task][run] after invoke: 0.000517, 0.000567, 0.000608
[task][run] after invoke: 0.000508, 0.000571, 0.000633
[task][run] after invoke: 0.000533, 0.000579, 0.000617
[task] print net output: 0.337575, 0.342671, 0.356567
[task] postproc: 0.000392, 0.000412, 0.000450
[task] postproc: 0.000383, 0.000412, 0.000458
[task] postproc: 0.000392, 0.000412, 0.000467
[task] postproc: 0.000392, 0.000421, 0.000467
[task] postproc: 0.000400, 0.000408, 0.000442
[task] label print: 1.393792, 1.427983, 1.502933
[task] wave proc done: 0.022633, 0.023071, 0.023933
[task] wave proc done: 0.022442, 0.022942, 0.023325
[task] wave proc done: 0.022342, 0.022962, 0.023625
[task] wave proc done: 0.022533, 0.023067, 0.023683
[task] wave proc done: 0.022608, 0.022917, 0.023450
[task] full runtime: 22.478884, 23.279484, 23.792351
MAX possible measruement: 17895.6
```

Implementation option 1 for `provisioner`. As expected, the only difference is the `wave proc done`, so post copy which is more due to the need to copy one more block.
f32 preprocessing with optimized quantization
```txt
BENCHMARK (min, mean, max)
[irq] start next DMA and stuff: 0.001508, 0.001642, 0.001717
[irq] invalidate: 0.003800, 0.003829, 0.003858
[irq] memcpy: 0.010833, 0.011279, 0.012658
[irq] callback: 0.001267, 0.001379, 0.001633
[task] before preproc: 0.000108, 0.000108, 0.000125
[task] preproc: 7.483475, 8.178845, 8.338117
[task] quantize: 0.015575, 0.015658, 0.016058
[task][run] run before invoke: 0.001792, 0.001967, 0.002283
[task][run] invoke: 13.570817, 13.577408, 13.583541
[task][run] after invoke: 0.000517, 0.000546, 0.000575
[task] print net output: 0.337800, 0.340675, 0.349667
[task] postproc: 0.000383, 0.000404, 0.000433
[task] label print: 1.394017, 1.451075, 1.502892
[task] wave proc done: 0.033808, 0.034633, 0.036300
[task] full runtime: 22.851658, 23.601347, 23.821583
MAX possible measruement: 17895.697266
```

f32 preprocessing with naive quantization
```txt
BENCHMARK (min, mean, max)
[task] preproc: 8.098250, 8.236879, 8.338258
[task] quantize: 0.028800, 0.028854, 0.029267
```

q31 preprocessing with naive quantization
Also it recognizes a most of the things similarly well, although it seems a bit more unsecure.
```txt
BENCHMARK (min, mean, max)
[task] preproc: 9.388599, 10.428620, 11.117766
[task] quantize: 0.029283, 0.030046, 0.031317
```

q15 preprocessing with naive quantization
Again, similarly good results.
```txt
BENCHMARK (min, mean, max)
[task] preproc: 6.700391, 7.684579, 8.319942
[task] preproc: 7.770067, 7.954700, 8.235608
[task] preproc: 7.771950, 8.009504, 8.241067
[task] quantize: 0.030808, 0.031654, 0.033400
[task] quantize: 0.030800, 0.031254, 0.031833
[task] quantize: 0.030800, 0.031092, 0.031475
```

q15 preprocessing with optimized q15 quantization
todo: so far this gives incorrect output, debug is needed
I might not do this, the q15 preproc is practically the same speed, the quantization is only a small fraction of this anyway. Also the naive way is probably almost as fast.

#### Shared, non_cacheable, speedy section

Added several sections into the linker script. They all serve different purposes. The shared section can be used for communication between the cores, it should have a configured MPU with the shared property. The non_cacheable section can be used for the DMA buffers, so that invalidation is not required. The speedy sectoin is set to DTCM, which can be a little bit faster than the cached memory. In this case it only helped around 1%.

After moving the model data and the allocation buffer of tflm to the speedy mem, the execution is a few percent faster:

```txt
BENCHMARK (min, mean, max)
[task] before preproc: 0.000092, 0.000108, 0.000117
[task] preproc: 6.819450, 7.943692, 8.252450
[task] quantize: 0.030683, 0.031125, 0.033033
[task][run] run before invoke: 0.001350, 0.001467, 0.001542
[task][run] invoke: 13.404592, 13.405554, 13.406983
[task][run] after invoke: 0.000475, 0.000504, 0.000533
[task] postproc: 0.000392, 0.000417, 0.000450
[task] wave proc done: 0.022508, 0.024013, 0.025158
[task] full runtime: 22.132433, 23.214828, 23.506817
MAX possible measruement: 17895.697266
```

Setting up the sections has a few steps. First they have to be written inside the linker script. There was a heap section, which has a size configured to check if the data fits, but I don't usually configure this, as the application changed quickly. This section always grows, and if it can't fit, then it grows into the secitons following it. This caused the my test to print wrong values. If the custom sections are allocated before this, then no error occurs.

Also some startup code has to be written, which initializes these sections, copies the initial values in case of `.data`, and initializes in case of `.bss`.

The shared section is configured to be shareable between the cores. Assertion should be added that it is at the same place on both cores. Practically at the start of D3 RAM. The sectoins are checked if they fit in the assumed mpu size.

The non-cacheable section uses normal memory that is not cacheable.

There is another section that coveres areas that are not inside the defualt memory map, and prohibites all access on them.

#### MPU configuration

The configured regions have a priority, 0 is the lowest, 15 is the highest, so e.g. new memory can be added as an exception from the default region that covers all default addresses mentioned in the previous section.

The TEX bit is only there to provide further information, usign it allows to configure the cache type, so if it should be write-back and write-allocate. Otherwise device and strictly-ordered memories can be set up.

#### MPU instead of invalidate

With MPU:

```txt
[irq] memcpy: 0.050825, 0.050992, 0.051825
[task] preproc: 8.623116, 8.827933, 9.104575
[task] quantize: 0.030475, 0.030871, 0.031158
[task][run] invoke: 13.595641, 13.598537, 13.603058
[task] wave proc done: 0.101525, 0.101579, 0.101650
```

With invalidate:

```txt
[irq] invalidate: 0.003783, 0.003796, 0.003817
[irq] memcpy: 0.010875, 0.011917, 0.012992
[task] preproc: 6.760533, 7.744237, 8.380116
[task] quantize: 0.030717, 0.031500, 0.032783
[task][run] invoke: 13.598566, 13.603829, 13.609176
[task] wave proc done: 0.022575, 0.022979, 0.023558
```

Invalidate is fast, and using the MPU slows down all other operations, which causes more slowdown, than the invalidate caused.

#### Top and bottom half of interrupt handling

Using deferred function call in FreeRTOS. The software timer daemon task executes the fucntions. It doesn't result in faster executtion, but less time is spent in interrupt handlers, which is desirable in case of more parallel tasks and so in embedded systems in general.

#### Performing the same actions on M4

It start with copying the provisioning part. One of the cores initializes the UART, this core cna use the reception with DMA. The other core only sets up the UART handle, thus can only send (no DMA).

After this the preprocessing can be performed on M4. Then the benchmark should show the difference between performance. The network should not be run here, as the earlier results show that it is not sensible to run on this slower core (3 times slower). The preprocessing is similarly 4 times slower.

__M4 measurements:__

Implemenetation option 3 (q15 preprocessing and naive quant)

```txt
BENCHMARK (min, mean, max) (in \[ms])
[irq] actual interrupt level: 0.002200, 0.002200, 0.002200
[irq] pend to bottom half begin: 0.006250, 0.006250, 0.006258
[irq] invalidate: 0.000192, 0.000192, 0.000192
[irq] memcpy: 0.024192, 0.025458, 0.026725
[irq] callback to task: 0.507308, 0.607550, 0.771258
[task] before preproc: 0.000233, 0.000233, 0.000233
[task] preproc: 23.546265, 25.713312, 27.028933
[task] quantize: 0.107200, 0.107358, 0.108067
[task] wave proc done: 0.053317, 0.053367, 0.054167
[task] full runtime (not running net): 23.707016, 25.874273, 27.189684
MAX possible measruement: 17895.697266
```

Implementation option 1 (f32 preprocessing with naive quant)

```txt
[task] preproc: 25.300066, 25.551458, 25.861557
[task] quantize: 0.053508, 0.053562, 0.054400
[task] full runtime (not running net): 25.407091, 25.658646, 25.968582
```

Implementation option 0 (f32 preprocessing with dsp quantization)

```txt
[task] preproc: 25.299967, 25.551275, 25.860550
[task] quantize: 0.049533, 0.049692, 0.050408
[task] full runtime (not running net): 25.403051, 25.654629, 25.964468
```

Couldn't measure the last implementation option, because the q31 code locks up the core in release mode. I didn't check the implementation, because on the other core this was the slowest option also, and because the float implementation is even faster than the quantized. It is much simpler, so I will be using that.

#### Communication

My previous calculation is aroun 80 us latency for sending the complete mfcc between the cores. It is less than 1 percent, not worth the optimization effort. So I will implement the simple FreeRTOS supported AMP communication (using Message Buffers).
