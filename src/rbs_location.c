#include "rbs/rbs_location.h"

#include <stdio.h>

#define RBS_LOC_CHILDREN_SIZE(cap) (sizeof(rbs_loc_children) + sizeof(rbs_loc_entry) * ((cap) - 1))

static void check_children_max(unsigned short n) {
  size_t max = sizeof(rbs_loc_entry_bitmap) * 8;
  if (n > max) {
    fprintf(stderr, "Too many children added to location: %d", n);
    exit(EXIT_FAILURE);
  }
}

static void ensure_children_capacity(rbs_allocator_t *allocator, rbs_location_t *loc) {
  if (loc->children == NULL) {
    rbs_loc_alloc_children(allocator, loc, 1);
  } else {
    if (loc->children->len == loc->children->cap) {
      size_t old_size = RBS_LOC_CHILDREN_SIZE(loc->children->cap);
      size_t new_size = old_size * 2;

      check_children_max(new_size);

      rbs_loc_children *new_children = rbs_allocator_realloc(allocator, loc->children, old_size, new_size, rbs_loc_children);
      new_children->cap = new_size;
      loc->children = new_children;
    }
  }
}

void rbs_loc_alloc_children(rbs_allocator_t *allocator, rbs_location_t *loc, int capacity) {
  check_children_max(capacity);

  loc->children = rbs_allocator_malloc_impl(allocator, RBS_LOC_CHILDREN_SIZE(capacity), alignof(rbs_loc_children));

  loc->children->len = 0;
  loc->children->required_p = 0;
  loc->children->cap = capacity;
}

void rbs_loc_add_required_child(rbs_allocator_t *allocator, rbs_location_t *loc, rbs_constant_id_t name, range r) {
  rbs_loc_add_optional_child(allocator, loc, name, r);

  unsigned short last_index = loc->children->len - 1;
  loc->children->required_p |= 1 << last_index;
}

void rbs_loc_add_optional_child(rbs_allocator_t *allocator, rbs_location_t *loc, rbs_constant_id_t name, range r) {
  ensure_children_capacity(allocator, loc);

  unsigned short i = loc->children->len++;
  loc->children->entries[i].name = name;
  loc->children->entries[i].rg = (rbs_loc_range) { r.start.char_pos, r.end.char_pos };
}

rbs_location_t *rbs_location_new(rbs_allocator_t *allocator, range rg) {
  rbs_location_t *location = rbs_allocator_alloc(allocator, rbs_location_t);
  *location = (rbs_location_t) {
    .rg = rg,
    .children = NULL,
  };

  return location;
}

