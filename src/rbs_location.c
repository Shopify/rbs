#include "rbs/rbs_location.h"

#include <stdlib.h>

#define RBS_LOC_CHILDREN_SIZE(cap) (sizeof(rbs_loc_children) + sizeof(rbs_loc_entry) * ((cap) - 1))

static rbs_loc_range rbs_new_loc_range(range rg) {
  rbs_loc_range r = { rg.start.char_pos, rg.end.char_pos };
  return r;
}

rbs_location_t *rbs_location_pp(const position *start_pos, const position *end_pos) {
  range rg = { *start_pos, *end_pos };
  rg.start = *start_pos;
  rg.end = *end_pos;

  return rbs_location_new(rg);
}

rbs_location_t *rbs_location_new(range rg) {
    rbs_location_t *location = (rbs_location_t *)malloc(sizeof(rbs_location_t));
    *location = (rbs_location_t) {
      .rg = rg,
      .children = NULL,
    };

    return location;
}

void rbs_loc_alloc_children(rbs_location_t *loc, int capacity) {
  size_t s = RBS_LOC_CHILDREN_SIZE(capacity);
  loc->children = malloc(s);

  loc->children->len = 0;
  loc->children->required_p = 0;
  loc->children->cap = capacity;
}

void rbs_loc_add_required_child(rbs_location_t *loc, rbs_constant_id_t name, range r) {
  rbs_loc_add_optional_child(loc, name, r);

  unsigned short last_index = loc->children->len - 1;
  loc->children->required_p |= 1 << last_index;
}

void rbs_loc_add_optional_child(rbs_location_t *loc, rbs_constant_id_t name, range r) {
  unsigned short i = loc->children->len++;
  loc->children->entries[i].name = name;
  loc->children->entries[i].rg = rbs_new_loc_range(r);
}
