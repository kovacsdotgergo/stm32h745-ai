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
