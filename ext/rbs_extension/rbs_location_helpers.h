#ifndef RBS__RBS_LOCATION_HELPERS_H
#define RBS__RBS_LOCATION_HELPERS_H

// This file contains pure-C types shared by both "location.h" and "rbs_location.h".

#include <stdint.h>
#include "lexer_helpers.h"

typedef uintptr_t MyID; // A stand-in for `ID` from `ruby.h`

typedef struct {
  int start;
  int end;
} rbs_loc_range;

typedef struct {
  MyID name;
  rbs_loc_range rg;
} rbs_loc_entry;

typedef unsigned int rbs_loc_entry_bitmap;

// The flexible array always allocates, but it's okay.
// This struct is not allocated when the `rbs_loc` doesn't have children.
typedef struct {
  unsigned short len;
  unsigned short cap;
  rbs_loc_entry_bitmap required_p;
  rbs_loc_entry entries[1];
} rbs_loc_children;

rbs_loc_range rbs_new_loc_range(range r);

#endif
