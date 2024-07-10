#!/usr/bin/env bash
debug=$1
nn_framework=$2

scripts_dir=$(dirname $0)
build_dir_m4=$($scripts_dir/project_paths.sh build_dir cm4 $debug $nn_framework) || exit 1
build_dir_m7=$($scripts_dir/project_paths.sh build_dir cm7 $debug $nn_framework) || exit 1
m7_executable=$($scripts_dir/project_paths.sh elf_name cm7) || exit 1
m4_executable=$($scripts_dir/project_paths.sh elf_name cm4) || exit 1

orange_clr="\x1b[38;5;209m"
yellow_clr="\x1b[38;5;184m"
light_grey_clr="\x1b[38;5;238m"
no_clr="\x1b[0m"

openocd \
    -f st_nucleo_h745zi.cfg \
    -c "program ${build_dir_m4}/${m4_executable} verify" \
    -c "program ${build_dir_m7}/${m7_executable} verify reset" \
    -c "shutdown" \
|& sed -e "s/\*\*.*\*\*/${light_grey_clr}&${no_clr}/" \
    -e "s/^Info :/${yellow_clr}&${no_clr}/" \
    -e "s/^Warn :/${orange_clr}&${no_clr}/"
