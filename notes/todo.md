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

* there is something wrong with the colorization script, the color code was printed before a note
* reentrant printf
* in the `.a` lib makefiles seperate build rules could be used for those objects, so that all of the inlcude paths are not used for other files. Of couse some of them are needed for the interface of the library, but if a part is unnecessary, then the build time could be imporved
* could generate `LDLIBS` and `LDFLAGS` from the libs list
* tio config file and vscode task
* move the `paths.sh` functionality into makefile, because it is aware or the paths. Programming and everything else should be done by a target in the makefile, then the paths script wouldn't even be needed
* docker
  * dependencies: arm bin, stlink, openocd, st-flash, freertos, ?srec_cat
  * decide how the container should be used, probably not as a devcontainer, it would be enough to build inside
  * ~~rights inside the nn container, should not run as root~~
* move the debug config into h745
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
* more frameworks
  * glow
  * generating cmsis nn code

* selecting a practical network
  * start off by using the nets used in the benchmarks
    * start off by using the nets in the repo, use the inputs already exported by including in the code
    * send the inputs via usb or uart
  * research the quantization aware training
  * use the kws streaming paper for selecting optianlly better nets and to use streaming for optimization
* speeding up the network
  * splitting and using IPC to execute in parallel
  * feeding the inputs thorough USB
  * talk about the possibilities, if one core doesn't have time to process, then the other executes the network ...
  * possible optimization is to use the DTCM for the data to be optimized, it was the default in this projcet, but after overflows i have moved to D1 (in theory, DTCM is the fastest)
* add ethernet for the outputs, signal something depending on the result of the classification

* ~~check the benchmark (was measurement before) code~~
* ~~run on all sets (train, val, test)~~
* implement all types of preprocessing on the hardware
  * ~~finalize the dependency generation script~~
* __postponed__: dump intermediate results
* __postponed__: check the cause of the difference
* run with wave inputs on the hardware
* measure all types of preprocessing
* merge the output transformation of the preprocessing and the input quantizaton of the net
* might be __IMPORTANT__: check if in theory the integer versions could saturate
* make streaming inference

* there are two cmsis directories, one in drivers, that st generated, the other in tflite third party. unite them, use the more recent if it builds and use it for all teh purposes (inside all the source code and to build the tflite lib)
* could use the dep discovery script for tflm, to remove files and only use them to compile

## Optimization

* when measured the speed of the preprocessing, the conversion of the input samples could be removed if the waveform is already stored in the proper format