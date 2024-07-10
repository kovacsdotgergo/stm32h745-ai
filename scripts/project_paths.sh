#!/usr/bin/env bash
# calculates the paths in the project

query=${1:-root_dir}
core=${2:-cm4}
debug=${3:-0}
nn_framework=${4:-tflm}

# validating and converting the config inputs
case $core in
    (cm4 | cm7) ;;
    (*) echo "Not valid value of \`core\`"; exit 1 ;;
esac
case $debug in
    (0 | release) debug_string=release ; debug=0 ;;
    (1 | debug) debug_string=debug ; debug=1 ;;
    (*) echo "Not valid value of \`debug\`"; exit 1 ;;
esac
case $nn_framework in
    (tflm | X-CUBE-AI) ;;
    (*) echo "Not valid value of \`nn_framework\`"; exit 1 ;;
esac
# `query` is validated in the main case

scripts_dir=$(dirname $0)
root_dir=$(realpath ${scripts_dir}/../src/h745)
makefile_dir=$root_dir/Makefile/${core^^} 
build_dir=$makefile_dir/build/${debug_string}/${nn_framework}

case $query in
    (root_dir) echo $root_dir ;;
    (makefile_dir) echo $makefile_dir ;;
    (build_dir | executable_dir) echo $build_dir ;;
    (elf_name) echo stm32h745-ai_${core^^}.elf ;;
    (arguments) echo core=$core debug=$debug nn_framework=$nn_framework ;; # can be used with eval to get the standardized value of the arguments
    (*) echo "Not valid option for the query requested" ; exit 1 ;;
esac