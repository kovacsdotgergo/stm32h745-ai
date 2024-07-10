#!/usr/bin/env bash
# builds the code for one of the cores

core=$1
debug=$2
nn_framework=$3

scripts_dir=$(dirname $0)
eval $($scripts_dir/project_paths.sh arguments $core $debug $nn_framework) || exit 1

# todo get paths
makefile_dir=$($scripts_dir/project_paths.sh makefile_dir $core) || exit 1
build_dir=$($scripts_dir/project_paths.sh build_dir $core $debug $nn_framework) || exit 1

# mkdir to have the base folder for tee
mkdir -p $build_dir
# build command
make -j4 -C $makefile_dir debug=$debug nn_framework=$nn_framework \
|& tee $build_dir/last_build.log | $scripts_dir/colorize.sh
