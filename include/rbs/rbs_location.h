#ifndef RBS__RBS_LOCATION_H
#define RBS__RBS_LOCATION_H

#include "ruby.h"
#include "lexer.h"

typedef struct rbs_location {
    VALUE cached_ruby_value;
    VALUE buffer;
    range rg;
} rbs_location_t;

rbs_location_t *rbs_location_new(VALUE buffer, range rg);

/**
 * Returns rbs_location_t struct with start/end positions.
 *
 * @param start_pos
 * @param end_pos
 * @return New rbs_location_t struct.
 * */
rbs_location_t *rbs_location_pp(VALUE buffer, const position *start_pos, const position *end_pos);

#endif
