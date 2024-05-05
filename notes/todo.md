# Short and long range plans

* merging the hex of bin or elf files
* loading the application for release mode
* loadign the application for debug mode
* use of the cross trigger interface
* openocd or stlink

* research about isa, nn acceleratos...
* try compilers

## Ai compilers

* cube ai
* tflite
* nvidia

## Building and utilities with lower prio

* change the source paths in makefile to help vscode locate the errors
* tio config file and vscode task
* colored make output [link](https://stackoverflow.com/questions/6436563/how-can-i-highlight-the-warning-and-error-lines-in-the-make-output)
* clang-format
* docker
  * dependencies: arm bin, stlink, openocd, st-flash, freertos, ?srec_cat
* read about starting udev in wsl to remove the one minute wait time
  * for this read about login and interactive shells, maybe only running the start comamnd in a login shell would solve the probelm -> in which startup file run the start command
* build task `build current` based on the inellisense configuration instead of build cm4 and cm7
* restructure and remove the makefile folder -> but then the merge script changes
* cross trigger interface?

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

* common
  * ~~add the timer on m4~~
  * ~~add printf on m4~~
    * ~~first try with bare uart~~
  * ~~add float printf on cm4~~
  * synchronize the print -> it doesn't even conflict right now, but hsem would be nice
  * setup the release build
  * update compiler flags
* cubeai
  * ~~timer from prev proj~~
  * ~~float printf~~
  * ~~run example~~
  * document interesting features
  * try the more useful features
  * setup CM4 as well
* tflite: first setup the proof of concept, then if it is ok, add the flags to optimize and make then toggleable to check mem consumption
  * cpp compilation
    * ~~constructors, startup, linker script~~
  * setup base
  * add flags: rtti, exception, developement
  * document more interesting features
  * try the more useful features
  * setup on CM4
* compare the frameworks
* try some more frameworks and document

* reentrant printf

## Today

To compose my thoughts.

* tflite start
* does build have to depend on headers and not only sources(trigger rebuild)

* summarize steps so far with tflm
* commit the buggy state

To search for solution:
* copy into a cubeide project
* check if mx has a tflite backend
* try with tutorial code and not helloworld
* try with different commit?