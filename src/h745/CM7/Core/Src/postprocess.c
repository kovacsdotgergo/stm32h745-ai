#include "postprocess.h"

const char* const postprocess_label_to_str[POSTPROCESS_LABEL_NUM] = {
    [DOWN] = "Down", [GO] = "Go",   [LEFT] = "Left",      [NO] = "No",
    [OFF] = "Off",   [ON] = "On",   [RIGHT] = "Right",    [STOP] = "Stop",
    [UP] = "Up",     [YES] = "Yes", [SILENCE] = "Silent", [UNKNOWN] = "Unknown",
};