# Notes to remember

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

If I use UART in write, then it won't be reentrant, so I think I should turn off interrupts inside the _write funciton because for this there are no available locks. Probabily using _write_r would also be OK, but it doesn't provide anithing plus because not usign the reent structure is the problem.

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
