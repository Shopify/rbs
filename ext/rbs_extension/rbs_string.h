#ifndef RBS__RBS_STRING_H
#define RBS__RBS_STRING_H

#include <stddef.h>

typedef struct {
  const char* start;
  const char* end;
} rbs_string_t;

extern const rbs_string_t RBS_STRING_NULL;

rbs_string_t rbs_string_new(const char* start, const char* end);
rbs_string_t rbs_string_offset(const rbs_string_t self, int offset);
size_t rbs_string_len(const rbs_string_t self);

#endif
