#ifndef POSTPROCESS_H
#define POSTPROCESS_H

enum postprocess_labels {
  DOWN = 0,
  GO,
  LEFT,
  NO,
  OFF,
  ON,
  RIGHT,
  STOP,
  UP,
  YES,
  SILENCE,
  UNKNOWN,
  POSTPROCESS_LABEL_NUM,
};

static const char* postprocess_label_to_str[] {
  [DOWN] = "Down", [GO] = "Go", [LEFT] = "Left", [NO] = "No", [OFF] = "Off",
  [ON] = "On", [RIGHT] = "Right", [STOP] = "Stop", [UP] = "Up", [YES] = "Yes",
  [SILENCE] = "Silent", [UNKNOWN] = "Unknown",
};

#endif  // POSTPROCESS_H