#!/usr/bin/env bash

makefile_dir=$(realpath $(dirname $0)/../src/h745/Makefile)
build_dir_m4_release=$makefile_dir/CM4/build/release
build_dir_m7_release=$makefile_dir/CM7/build/release
m7_executable=stm32h745-ai_CM7.elf
m4_executable=stm32h745-ai_CM4.elf

orange_clr="\x1b[38;5;209m"
yellow_clr="\x1b[38;5;184m"
light_grey_clr="\x1b[38;5;238m"
no_clr="\x1b[0m"

openocd \
    -f st_nucleo_h745zi.cfg \
    -c "program ${build_dir_m4_release}/${m4_executable} verify" \
    -c "program ${build_dir_m7_release}/${m7_executable} verify reset" \
    -c "shutdown" \
|& sed -e "s/\*\*.*\*\*/${light_grey_clr}&${no_clr}/" \
    -e "s/^Info :/${yellow_clr}&${no_clr}/" \
    -e "s/^Warn :/${orange_clr}&${no_clr}/"
