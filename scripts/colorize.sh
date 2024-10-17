#!/usr/bin/env bash

# colorize script from https://stackoverflow.com/questions/5732562/improving-g-output
# while read x ; do echo $x ; done \
# | sed -e "s/.*error:.*/\x1b[1;36m&\x1b[0m/" \
# -e "s/.*warning:.*/\x1b[1;36m&\x1b[0m/" \
# -e "s/^\(.*\)\(required from\)/\x1b[1;36m\1\x1b[0mnote: \2/" \
# -e "s/^\(.*\)\(In instantiation of\)/\x1b[1;36m\1\x1b[0mnote: \2/" \
# -e "s/^\(.*\)\(In member\)/\x1b[1;36m\1\x1b[0mnote: \2/" \
# | sed -e "s/error:/\x1b[1;31m&\x1b[1;36m/" \
# -e "s/warning:/\x1b[1;35m&\x1b[1;36m/" \
# -e "s/note:/\x1b[1;30m&\x1b[0m/"

dark_grey="\x1b[90m"
none="\x1b[0m"
red="\x1b[38;5;209m"
bold_dark_red="\x1b[1;38;5;088m"
bold_orange="\x1b[1;38;5;202m"
bold_yellow="x1b[1;38;5;184m"
bold_light_orange="\x1b[1;38;5;173m"
light_green="\x1b[38;5;70m"
light_yellow="\x1b[38;5;178m"
# colorizing the output of the non verbose Makefiles
# TODO: the numbers part might have to removed
while read x; do echo $x ; done \
| sed -e "s/^\(compiling\|linking\|archiving\)\(.*\)/${dark_grey}&${none}/" \
    -e "s/\b\([-a-zA-Z]*\(objcopy\|size\|objdump\)\)/${light_green}&${none}/" \
    -e "s/make:/${red}&${none}/" \
    -e "s/.*error:.*/${bold_light_orange}&${none}/" \
    -e "s/.*warning:.*/${bold_light_orange}&${none}/" \
    -e "s/^\(.*\)\(required from\)/${bold_light_orange}\1${none}note: \2/" \
    -e "s/^\(.*\)\(In instantiation of\)/${bold_light_orange}\1${none}note: \2/" \
    -e "s/^\(.*\)\(In member\)/${bold_light_orange}\1${none}note: \2/" \
| sed -e "s/error:/${bold_dark_red}&${bold_light_orange}/" \
    -e "s/warning:/${bold_orange}&${bold_light_orange}/" \
    -e "s/note:/${bold_yellow}&${none}/"

# numbers:
# -e "s/\b[0-9a-fA-F]*[0-9][0-9a-fA-F]*\b/${light_yellow}&${none}/g"