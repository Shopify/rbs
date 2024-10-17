#ifndef RBS__TEMP_UTILS_H
#define RBS__TEMP_UTILS_H

#include "ruby.h"
#include "rbs_string.h"
#include "rbs_buffer.h"
#include "rbs_location.h" // Thew new pure-C location, not to be confused with "location.h"

VALUE rbs_string_to_ruby_str(const rbs_string_t);
rbs_string_t rbs_string_from_ruby_str(const VALUE);

VALUE rbs_buffer_copy_into_ruby_buffer(const rbs_buffer_t);
rbs_buffer_t rbs_buffer_copy_from_ruby_buffer(VALUE);

// VALUE rbs_buffer_wrap_into_ruby_buffer(const rbs_buffer_t);
// rbs_buffer_t rbs_buffer_wrap_from_ruby_buffer(const VALUE);

VALUE rbs_buffer_content_ruby_str(const rbs_buffer_t);

VALUE rbs_location_wrap_into_ruby_obj(rbs_location_t *);

#endif
