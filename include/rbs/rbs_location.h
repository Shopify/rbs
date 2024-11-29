#ifndef RBS__RBS_LOCATION_H
#define RBS__RBS_LOCATION_H

#include "ruby.h"
#include "lexer.h"

typedef struct rbs_location {
    VALUE cached_ruby_value;
    range rg;
} rbs_location_t;

rbs_location_t *rbs_location_new(range rg);
void rbs_loc_alloc_children(rbs_location_t *loc, int size);
void rbs_loc_add_required_child(rbs_location_t *loc, ID name, range r);
void rbs_loc_add_optional_child(rbs_location_t *loc, ID name, range r);

/**
 * Returns rbs_location_t struct with start/end positions.
 *
 * @param start_pos
 * @param end_pos
 * @return New rbs_location_t struct.
 * */
rbs_location_t *rbs_location_pp(const position *start_pos, const position *end_pos);

#endif
