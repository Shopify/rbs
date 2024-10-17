#ifndef RBS__RBS_BUFFER_H
#define RBS__RBS_BUFFER_H

#include "rbs_string.h"

typedef struct {
  // TODO: make const?
  rbs_string_t name;
  rbs_string_t content;
} rbs_buffer_t;

extern const rbs_buffer_t RBS_BUFFER_NULL;

rbs_buffer_t rbs_buffer_new(const rbs_string_t name, const rbs_string_t content);

#endif
