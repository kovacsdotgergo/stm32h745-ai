# Short and long range plans

* finish off the network tests
* choose a useful net
* feeding it
* optimization
* ethernet

## Editor

* suggest
* paramter hints
* all hover navigation
* [this video might help](https://www.youtube.com/watch?v=2KcX_SdtHz0)

## Building and utilities with lower prio

* reentrant printf
* tio config file and vscode task
* move the `paths.sh` functionality into makefile, because it is aware or the paths. Programming and everything else should be done by a target in the makefile, then the paths script wouldn't even be needed
* docker
  * dependencies: arm bin, stlink, openocd, st-flash, freertos, ?srec_cat
* build task `build current` based on the inellisense configuration instead of build cm4 and cm7
* restructure and remove the makefile folder -> but then the merge script changes
* cross trigger interface?
* ~~make the openocd startup cleaner (no messages about already halting and change timeout)~~
  * I couldn't solve this, the timeout message is still there when adding the `set remotetimeout` command in the `launch.json` in `preLaunchCommands?`.
  * I have checked if the command is actually called by using the verbose gdb messages in Cortex Debug
* ~~add correct handling of dependecies to makefiles~~
* ~~change the source paths in makefile to help vscode locate the errors~~
* ~~colored make output [link](https://stackoverflow.com/questions/6436563/how-can-i-highlight-the-warning-and-error-lines-in-the-make-output)[script with sed](https://stackoverflow.com/questions/5732562/improving-g-output)~~
* ~~clang-format~~
* ~~read about starting udev in wsl to remove the one minute wait time~~
  * ~~for this read about login and interactive shells, maybe only running the start comamnd in a login shell would solve the probelm -> in which startup file run the start command~~

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
  * ~~setup the release build~~
  * update compiler flags

* cubeai
* tflite
  * compilation
  * both cores
  * measurement
* glow

* selecting a practical network
* speeding up the network
  * splitting and using IPC to execute in parallel
  * feeding the inputs thorough USB
  * talk about the possibilities, if one core doesn't have time to process, then the other executes the network ...
* add ethernet for the outputs, signal something depending on the result of the classification

## To finish off the tflite debug

* ~~to continue: remove the google test net and try all own test models~~
* makefile:
  * ~~correct dependency~~
  * ~~colored make~~
  * ~~logging~~
* cm4 also
  * ~~colorization of compilation~~
  * ~~build all task~~
  * ~~common elements in include makefiles~~
* ~~PROJECT_GENERATION macro implementation~~
  * using the core timer is possible, but I still used the hardware timer
* ~~cpp compilation~~
* ~~measure performance~~
* integrate both in one project

* more frameworks
  * glow
  * generating cmsis nn code
