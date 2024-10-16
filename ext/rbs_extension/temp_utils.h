#ifndef RBS__TEMP_UTILS_H
#define RBS__TEMP_UTILS_H

#include "ruby.h"
#include "rbs_string.h"
#include "rbs_buffer.h"

VALUE rbs_string_to_ruby_str(const rbs_string_t);
rbs_string_t rbs_string_from_ruby_str(const VALUE);

VALUE rbs_buffer_to_ruby_buffer(const rbs_buffer_t);
VALUE rbs_buffer_content_ruby_str(const rbs_buffer_t);

#endif
