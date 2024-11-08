#ifndef RBS__RBS_STRING_BRIDGING_H
#define RBS__RBS_STRING_BRIDGING_H

#include "ruby.h"
#include "rbs/rbs_string.h"

/**
 * Returns a new shared rbs_string_t from the given Ruby string.
 */
rbs_string_t rbs_string_from_ruby_string(VALUE ruby_string);

/**
 * Returns a new Ruby string from the given rbs_string_t.
 */
VALUE rbs_string_to_ruby_string(rbs_string_t *self);

#endif
