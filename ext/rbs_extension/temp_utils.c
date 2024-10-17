#include "temp_utils.h"

#include "constants.h"
#include "location.h" // The Ruby-based RBS::Location class
#include "ruby/encoding.h"

VALUE rbs_string_to_ruby_str(const rbs_string_t input) {
  // return rb_enc_str_new(input.start, rbs_string_len(input), rb_utf8_encoding());
  return rb_str_new_static(input.start, rbs_string_len(input));
}

rbs_string_t rbs_string_from_ruby_str(const VALUE input) {
  return rbs_string_new(RSTRING_PTR(input), RSTRING_END(input));
}

VALUE rbs_buffer_content_ruby_str(const rbs_buffer_t input) {
  return rbs_string_to_ruby_str(input.content);
}

rbs_buffer_t rbs_buffer_copy_from_ruby_buffer(const VALUE input) {
  return (rbs_buffer_t) {
    .name = rbs_string_from_ruby_str(rb_funcall(input, rb_intern("name"), 0)),
    .content = rbs_string_from_ruby_str(rb_funcall(input, rb_intern("content"), 0))
  };
}

VALUE rbs_buffer_copy_into_ruby_buffer(const rbs_buffer_t input) {
  VALUE kwargs = rb_hash_new();
  rb_hash_aset(kwargs, ID2SYM(rb_intern("name")), rbs_string_to_ruby_str(input.name));
  rb_hash_aset(kwargs, ID2SYM(rb_intern("content")), rbs_string_to_ruby_str(input.content));
  VALUE argv[1] = {kwargs};
  VALUE buffer = rb_funcallv_kw(RBS_Buffer, rb_intern("new"), 1, argv, RB_PASS_KEYWORDS);
  return buffer;
}

// VALUE rbs_buffer_wrap_into_ruby_buffer(const rbs_buffer_t) {

// }
// rbs_buffer_t rbs_buffer_wrap_from_ruby_buffer(const VALUE) {

// }

#define RBS_LOC_CHILDREN_SIZE(cap) (sizeof(rbs_loc_children) + sizeof(rbs_loc_entry) * ((cap) - 1))

VALUE rbs_location_to_ruby_loc(const rbs_location_t input) {
  VALUE ruby_buffer = rbs_buffer_copy_into_ruby_buffer(input.buffer);
  VALUE ruby_loc = rbs_new_location_from_loc_range(ruby_buffer, input.range);

  if (input.children != NULL) { // Copy over the children, if any
    rbs_loc *loc = rbs_check_location(ruby_loc);
    rbs_loc_alloc_children(loc, input.children->cap);
    memcpy(loc->children, input.children, RBS_LOC_CHILDREN_SIZE(input.children->cap));
  }

  return ruby_loc;
}
