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

Task can be set up to provide shourtcuts to launching programs or run scripts. I set up tasks for building, probing the board, and flashing. More options can be added to launch a program monitoring serial communication.

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

todo

## Optimization techniques for NNs

todo: Quantization is somewhat described here in the cubeai documentation under 'Quantized models support'. this will be important for the optimization part of my task

### Microcontrollers

todo

### Instruction sets

todo

### Neural net accelerators

todo

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
