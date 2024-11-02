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

extern const char* const postprocess_label_to_str[POSTPROCESS_LABEL_NUM];

#endif  // POSTPROCESS_H