#include "rbs_location.h"

rbs_location_t rbs_location_new(const rbs_buffer_t buffer, const rbs_loc_range range) {
    return (rbs_location_t) {.buffer = buffer, .range = range, .children = NULL};
}

rbs_location_t rbs_location_pp2(const rbs_buffer_t buffer, const position *start, const position *end) {
  rbs_loc_range range = { .start = start->char_pos, .end = end->char_pos };
  return rbs_location_new(buffer, range);
}
