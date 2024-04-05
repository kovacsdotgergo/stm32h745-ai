# Short and long range plans

* merging the hex of bin or elf files
* loading the application for release mode
* loadign the application for debug mode
* use of the cross trigger interface
* openocd or stlink

## Next steps

* ~~compare to prev project~~
* make a starter project with freertos
  * ~~using the generated mx project~~
  * move (update) the drivers as well after running
  * update to newer version with the help of the demo
  * remove everything unneccesary (e.g. cmsis os...)
* update cmsis to the version with the correct cache functions
* finalize vscode setup after running correctly
  * launch
  * tasks (programming)
  * intellisense from new makefile
* document the setup until this point, the os, the cmsis, the developement process

* research about isa, nn acceleratos...
* try compilers

## Ai compilers

* cube ai
* tflite
* nvidia

## Building and utilities

* docker
  * dependencies: arm bin, stlink, openocd, st-flash, freertos, ?srec_cat
* clang-format
* read about starting udev in wsl to remove the one minute wait time
  * for this read about login and interactive shells, maybe only running the start comamnd in a login shell would solve the probelm -> in which startup file run the start command
* build task `build current` based on the inellisense configuration instead of build cm4 and cm7
* restructure and remove the makefile folder -> but then the merge script changes
* cross trigger interface?
