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

dark_grey_clr="\x1b[90m"
no_clr="\x1b[0m"
red_clr="\x1b[38;5;209m"
light_green_clr="\x1b[38;5;70m"
# colorizing the output of the non verbose Makefiles
while read x; do echo $x ; done \
| sed -e "s/^\(compiling\|linking\|archiving\)\(.*\)/${dark_grey_clr}&${no_clr}/" \
    -e "s/\b\([-a-zA-Z]*\(objcopy\|size\|objdump\)\)/${light_green_clr}&${no_clr}/" \
    -e "s/make:/${red_clr}&${no_clr}/"

# -e "s/\s[0-9a-fA-F]*[0-9][0-9a-fA-F]*\s/\x1b[38;5;178m&\x1b[0m/"