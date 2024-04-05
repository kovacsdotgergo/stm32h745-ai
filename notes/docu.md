# Documentation of progress

## Useful resources

* merging two hex files [link arm doc](https://developer.arm.com/documentation/ka004500/latest/) [link doc](https://srecord.sourceforge.net/man/man1/srec_cat.1.html)

* c cpp properties different configurations for the two core [link](https://code.visualstudio.com/docs/cpp/c-cpp-properties-schema-reference)

* debug usign vscode [link](https://hbfsrobotics.com/blog/configuring-vs-code-arm-development-stm32cubemx) [another blog link](https://wiki.octanis.org/stm32/vscode) [tutorial link](https://www.youtube.com/watch?v=g2Kf6RbdrIs)

* cubeai and tflite tutorial [link](https://www.digikey.com/en/maker/projects/tinyml-getting-started-with-stm32-x-cube-ai/f94e1c8bfc1e4b6291d0f672d780d2c0)

* contex-debug extension documentation [link](https://github.com/Marus/cortex-debug/wiki#vscode-settings-for-cortex-debug)

## Building and debugging

Currently using openocd to debug. todo finish when the binaries are correctg

### Flashing

The texane stlink couldn't flash the memory of the M4 core, still the `st-info` tool works that can be used to check the state of the board. Besides this, for flashing the ST tool could be used as an alternative. This is capable of connecting after powering down? the device, which is usefult when the onboard stlink can't see the microcontroller. I could successfully erase the flash memory, but after using this setup to flash both cores, the code wouldn't start properly.

## Research

todo

### Microcontrollers

todo

### Instruction sets

todo

### Neural net accelerators

todo
