#include "rbs_location.h"
#include "rbs_common.h"

#include <stdlib.h>

// private "methods"

#define RBS_LOC_CHILDREN_SIZE(cap) (sizeof(rbs_loc_children) + sizeof(rbs_loc_entry) * ((cap) - 1))

static void check_children_max(unsigned short n) {
  size_t max = sizeof(rbs_loc_entry_bitmap) * 8;
  if (n > max) {
    rbs_fatal_error("Too many children added to location: %d", n);
  }
}

void rbs_location_alloc_children(rbs_location_t *self, unsigned short capacity) {
  check_children_max(capacity);

  size_t s = RBS_LOC_CHILDREN_SIZE(capacity);
  self->children = malloc(s);

  self->children->len = 0;
  self->children->required_p = 0;
  self->children->cap = capacity;
}

// public "methods"

rbs_location_t rbs_location_new(const rbs_buffer_t buffer, const rbs_loc_range range) {
    return (rbs_location_t) {.buffer = buffer, .range = range, .children = NULL};
}

rbs_location_t rbs_location_new_from_lexer_range(const rbs_buffer_t buffer, const range range) {
    return rbs_location_new(buffer, rbs_new_loc_range(range));
}

rbs_location_t rbs_location_pp2(const rbs_buffer_t buffer, const position *start, const position *end) {
  rbs_loc_range range = { .start = start->char_pos, .end = end->char_pos };
  return rbs_location_new(buffer, range);
}
