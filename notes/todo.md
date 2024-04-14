# Short and long range plans

* merging the hex of bin or elf files
* loading the application for release mode
* loadign the application for debug mode
* use of the cross trigger interface
* openocd or stlink

* research about isa, nn acceleratos...
* try compilers

## Next steps

* ~~compare to prev project~~
* ~~make a starter project with freertos~~
  * ~~using the generated mx project~~
  * ~~move (update) the drivers as well after running~~
  * ~~update hal sys timer~~
  * ~~fix script for deletions~~
  * ~~fix the newlib stuff, syscalls...~~
  * ~~update to newer version with the help of the demo~~
  * ~~printf~~
  * ~~remove everything unneccesary (e.g. cmsis os...)~~
* ~~update cmsis to the version with the correct cache functions~~
  * only moved the cache update
* ~~finalize vscode setup after running correctly~~
  * ~~launch~~
  * ~~tasks (programming)~~
  * ~~itellisense from new makefile~~
* document the setup until this point, the os, the cmsis, the developement process

## Ai compilers

* cube ai
* tflite
* nvidia

## Building and utilities with lower prio

* docker
  * dependencies: arm bin, stlink, openocd, st-flash, freertos, ?srec_cat
* clang-format
* read about starting udev in wsl to remove the one minute wait time
  * for this read about login and interactive shells, maybe only running the start comamnd in a login shell would solve the probelm -> in which startup file run the start command
* build task `build current` based on the inellisense configuration instead of build cm4 and cm7
* restructure and remove the makefile folder -> but then the merge script changes
* cross trigger interface?
* tio config file and vscode task
* colored make output [link](https://stackoverflow.com/questions/6436563/how-can-i-highlight-the-warning-and-error-lines-in-the-make-output)
