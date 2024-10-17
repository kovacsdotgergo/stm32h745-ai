# Previous hard to fix bugs

## Power supply

* the nucleo144 board uses the direct smps supply, for the maximum clock frequencies VOS0 has to be used
* previously in cubeide to enable this rev.V had to be selected, otherwise only VOS1 and above are available

## Clock source

* the hse clock input is used, the source is the MCO output from the onboard stlink
* this is set to 8 MHz, which is used when configuring the clock tree
* IMPORTANT: when updating the stlink firmware, the MCO output has to remain 8 MHz

## Cache maintanence

If cleaning the cache will be neccessary, the CMSIS version should be checked because of this [issue](https://github.com/ARM-software/CMSIS_5/issues/620). I am not sure if everything in CMSIS can be updated, because the ST Hal could break.

## Objectfiles overwriting

The ST makefile I used as a base uses a flat hierarchy inside the build folder, the tensorflow library does not. There are multiple files with the same name inside tensorflow, so when copiling into the object files with the same name, some of them were lost. Files with the same name do not cause a problem when including headers, because everything is included with the full relative path in tensorflow (e.g `tensorflow/lite/kernels/op_macros.h`).

As a solution I will be switching to a hierarchical build folder.

## Tflite bringup, private variable not initialized

When using a 0 initialized private variable of a class (`private: int num_events_ = 0;`), the variable is not actually intialized, and when using it to index an array, HardFault happens. Currently I need to check how the initialization of these classes should happen.

The variable is outside the stack, because the tflite::MicroProfiler class has 4096 events by default, which results in a class size of 81928. Checking the position of the variable on the stack:

```shell
p (char*)pxCurrentTCB->pxEndOfStack - (char*)&profiler->num_events_
$33 = -43960
```

It is outside the stack, maybe this is why the value was corrupted.

## Stack overflows

When starting a proper net, it required a large stack. This is allocated by a malloc in the freertos task creation code. The TCB pointers are only updated on init, so the current sp should always be observed. First I got hard faults when the stack overflowed, later the code got stuck in an unrelated interrupt. When checking the `(StackType_t*)<current sp read from the debugger variables view> - pxCurrentTCB->pxStack`, it showed overflow. The freertos stack checking didn't catch this, and it didn't usually work before either.

I think the previous stack overflows happened, because with a too small stack allocated, when it overflowed, it grew past the start address of the ram. In the linker script the DTCM ram is used, below which is a reserved region, I might have written that.