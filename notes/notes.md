# Notes to remember

Misc notes about different topics to document understanding.

## Merging binaries

``` bash
command = "srec_cat"
args = [
    "${HEX_FILE_0}", "-intel",
    "${HEX_FILE_1}", "-intel",
    "-o", "${MERGE_HEX}", "-intel",
    "-Line_Length", "44"
]
```

* merging two hex files [link arm doc](https://developer.arm.com/documentation/ka004500/latest/) [link doc](https://srecord.sourceforge.net/man/man1/srec_cat.1.html)

## Profiling stm32

* everything by hand, article: [link](https://interrupt.memfault.com/blog/profiling-firmware-on-cortex-m)
  * where I found the link: [link](https://stackoverflow.com/questions/77628950/visualize-profiling-data-from-an-stm32-cortex-m4-cpu)
* gprof [link](https://mcuoneclipse.com/2015/08/23/tutorial-using-gnu-profiling-gprof-with-arm-cortex-m/)

## Newlib

* introduction article [link](https://hackaday.com/2021/07/19/the-newlib-embedded-c-standard-library-and-how-to-use-it/)
* documentation about the required functionality [link](https://sourceware.org/newlib/libc.html#Stubs)
* article about FreeRTOS and reentrancy [**link**](https://nadler.com/embedded/newlibAndFreeRTOS.html)
  * github link that helps understanding the configuration [link](https://github.com/DRNadler/FreeRTOS_helpers/issues/3)
* about newlib [link](https://www.embedded.com/embedding-with-gnu-newlib/)

As I understand so far, the reentrant functions are calling the regularly named ones by default. For malloc there are the locks that have to be implemented. To handle errno there is a macro defined. So the versions with _r only have to be used in special cases not covered by these?

If I use UART in write, then it won't be reentrant, so I think I should turn off interrupts inside the _write funciton because for this there are no available locks. Probabily using_write_r would also be OK, but it doesn't provide anithing plus because not usign the reent structure is the problem.

### Malloc inside ISR

It's problematic due to reentrancy, malloc is usually not safe for this purpose, so mallocing messes up the memory pool.

It can be solved if reentry into malloc is not possible, e.g. wrapping malloc into a call with disabling interrupts.

## Tracking a call to a function

* --trace_symbol=x linker option will list all the places where the function is called
* also for more insight, a breakpoint on the function can also be used
* [link](https://community.st.com/t5/stm32cubemx-mcus/bug-cubemx-freertos-projects-corrupt-memory/m-p/267070)
* --wrap symbol option of ld [link](https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_node/ld_3.html)

## Deferred interrupt handling in FreeRTOS

Handling the interrupts in a task not in the ISR [link](https://www.freertos.org/deferred_interrupt_processing.html).

## FreeRTOS debug

There is a tab for xRTOS, so probably VSCode can also do RTOS aware debugging.

FreeRTOS guide: [link](https://www.freertos.org/2021/01/using-visual-studio-code-for-freertos-development.html)

There is a global pointer to the current TCB where a pointer to the current task. I used this to check the offset of the variables on the stack when it overflowed.

## Semihosing

Semihosting is implemented by certain asm instructions. These instructions take a code as a parameter. Depending on this parameter the semihosting operation is selected. Operations are the library functions required by the standard library e.g. isatty, write. These instructions are using the debugger to perform these low level operations. [arm semihosing docu](https://developer.arm.com/documentation/dui0471/g/Semihosting)

To make semihosting work, the debugger and the required linker flags have to be set up. [example blog](https://fastbitlab.com/microcontroller-embedded-c-programming-lecture-51-testing-printf-over-openocd-semihosting/)

## SWO and ITM

* [SWO related commands in openocd](https://openocd.org/doc/html/Architecture-and-Core-Commands.html)
* [SWO and ITM setup on some blog](https://embedthreads.com/how-to-use-printf-on-stm32-using-itmswo-line/)
* [another SWO stup](https://fastbitlab.com/microcontroller-embedded-c-programming-lecture-48-embedded-hello-world/)
* ARM CoreSight SoC-400 - technical reference about some debug infrastructure, including TPIU
* [solution in CubeIDE](https://lowlevelcode.com/stm32-debugging-with-printf-by-using-swv-or-openocd/)

## Compiler

Some bad warnings are cast between incompatible pointers and implicit function declarations, these should be turned to errors.

### Flags for embedded projects

* [great article](https://interrupt.memfault.com/blog/best-and-worst-gcc-clang-compiler-flags)

The cmake file for building for arm cores are in ethos-u-core-platform/cmake/toolchain/arm-none-eabi-gcc.cmake. This sets several flags. It is inlcuded when building for the core.

The tflite-micro/tensorflow/lite/micro/tools/make/targets/cortex_m_generic_makefile.inc is similar, sets up everything used for the core, also sets a bunch of compiler flags, but it is a Makefile.

## C++

* RTTI
  * [dynamic cast and RTTI](https://pvs-studio.com/en/blog/posts/cpp/0998/)
* exceptions
  * [about the hardness of exceptions](https://devblogs.microsoft.com/oldnewthing/20050114-00/?p=36693)
* STL

### Cpp embedded startup

The only thing I found for the linker script and the startup code to do are calling the constructor for static objects. The destructor for these should never be called as the program does not exit. The function that performs this according to the comment in the startup code is the `__libc_init_array` function. This calls functions that are set up wiht the proper attribute. It might also call the constructors?

[Gcc documentation of the initialization](https://gcc.gnu.org/onlinedocs/gcc-7.1.0/gccint/Initialization.html).

A problem might also occur due to the order of the static constructors and initializations:
[about the init arrays used by GCC](https://stackoverflow.com/questions/15265295/understanding-the-libc-init-array)
 -> it might lead to the [static initialization order problem](https://isocpp.org/wiki/faq/ctors#static-init-order)

I have not yet found anything else required for cpp (no additional sections or startup code).

### Reading about C++

#### Unwind tables

[Stackoverflow question about the flag](https://stackoverflow.com/questions/53102185/what-exactly-happens-when-compiling-with-funwind-tables)

There is a flag to enable (usually it is on) generating these unwind tables. It is used to hold information to be able to unwind the stack at any point where exception can be thrown inside the function. Usually in C there are only normal control flow points where the stack has to be unwinded. Here we might have to do it anywhere at exception. Also we might have to unwind more calls until the first catch block.

The tables is stored in a seperate section. Each function with exceptions has its own unwind table, so it can only propagate back through functions with unwind tables (logical noexcept requirement). There is a callback function implemented in the compiler that is able to unwind form any point.

In assembly to be compatible functions have to follow the exeption handling abi (e.g. [ARM](https://developer.arm.com/documentation/dui0473/m/writing-arm-assembly-language/exception-tables-and-unwind-tables))

#### CRTP (Curiously Recurring Template Pattern)

Can be used to implement static polimorphism.

#### Variable templates (cpp14)

The type of the variable is the template. [cppreference](https://en.cppreference.com/w/cpp/language/variable_template)

#### If stattement with initializer

```cpp
if (init, condition) {
  // init availabe
}
// init out of scope

// instead of
init
if (condition) {
  // init available
}
// init still available
```

It limits the scope of the init, it can not be accessed outside the init block. [brief summary](https://www.tutorialspoint.com/cplusplus17-if-statement-with-initializer)

### Mixing C and C++

* [gist about adding cpp sources to makefile](https://gist.github.com/SteelPh0enix/7d2670537be6ad65e190bb1e91e87444)
* [talk about embedded cpp](https://www.youtube.com/watch?v=wLq-5lBc7x4)

Extern "C" is required for the FreeRTOS hooks and the init functions that are called in e.g. main.

## Hard fault

todo: write about the article and the hard fault analyzer

## Checking task stack
todo: move this section below anything relevant
`p pxCurrentTCB->pxTopOfStack` stack pointer
`p pxCurrentTCB->pxStack` end of stack
`p pxCurrentTCB->pxEndOfStack` beginning of stack
`(char*)pxCurrentTCB->pxEndOfStack - <variable>` to check the position of the variable on the stack