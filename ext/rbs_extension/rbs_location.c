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

static void check_children_cap(rbs_location_t *self) {
  if (self->children == NULL) {
    rbs_location_alloc_children(self, 1);
  } else {
    if (self->children->len == self->children->cap) {
      check_children_max(self->children->cap + 1);
      size_t s = RBS_LOC_CHILDREN_SIZE(++self->children->cap);
      self->children = realloc(self->children, s);
    }
  }
}

// Constructors

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

// Methods

void rbs_location_alloc_children(rbs_location_t *self, const unsigned short capacity) {
  check_children_max(capacity);

  size_t s = RBS_LOC_CHILDREN_SIZE(capacity);
  self->children = malloc(s);

  self->children->len = 0;
  self->children->required_p = 0;
  self->children->cap = capacity;
}

void rbs_location_add_required_child(rbs_location_t *self, const MyID name, const range r) {
  check_children_cap(self);

  unsigned short i = self->children->len++;
  self->children->entries[i].name = name;
  self->children->entries[i].rg = rbs_new_loc_range(r);

  self->children->required_p |= 1 << i;
}

void rbs_location_add_optional_child(rbs_location_t *self, const MyID name, const range r) {
  check_children_cap(self);

  unsigned short i = self->children->len++;
  self->children->entries[i].name = name;
  self->children->entries[i].rg = rbs_new_loc_range(r);
}
