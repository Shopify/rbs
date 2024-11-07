#include "rbs/rbs_location.h"

rbs_location_t *rbs_location_pp(VALUE buffer, const position *start_pos, const position *end_pos) {
  range rg = { *start_pos, *end_pos };
  rg.start = *start_pos;
  rg.end = *end_pos;

  return rbs_location_new(buffer, rg);
}
