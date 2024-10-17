#include "temp_utils.h"

#include "constants.h"
#include "location.h" // The Ruby-based RBS::Location class
#include "ruby/encoding.h"

VALUE rbs_string_to_ruby_str(rbs_string_t input) { // TODO: take this by pointer, so the `cached_ruby_string` is updated in the caller
  if (input.cached_ruby_string == Qnil) {
    input.cached_ruby_string = rb_str_new_static(input.start, rbs_string_len(input));
  }
  return input.cached_ruby_string;
  // return rb_enc_str_new(input.start, rbs_string_len(input), rb_utf8_encoding());
}

rbs_string_t rbs_string_from_ruby_str(VALUE input) {
  return (rbs_string_t) { .start = StringValueCStr(input), .end = RSTRING_END(input), .cached_ruby_string = input };
}

VALUE rbs_buffer_content_ruby_str(const rbs_buffer_t input) {
  return rbs_string_to_ruby_str(input.content);
}

rbs_buffer_t rbs_buffer_copy_from_ruby_buffer(const VALUE input) {
  VALUE name = rb_funcall(input, rb_intern("name"), 0);
  VALUE pathname_class = rb_const_get(rb_cObject, rb_intern("Pathname"));

  VALUE name_string = rb_obj_is_kind_of(name, pathname_class)
    ? rb_funcall(name, rb_intern("to_s"), 0)
    : name;

  return rbs_buffer_new(
    rbs_string_from_ruby_str(name_string),
    rbs_string_from_ruby_str(rb_funcall(input, rb_intern("content"), 0))
  );
}

VALUE rbs_buffer_copy_into_ruby_buffer(const rbs_buffer_t input) {
  VALUE kwargs = rb_hash_new();
  rb_hash_aset(kwargs, ID2SYM(rb_intern("name")), rbs_string_to_ruby_str(input.name));
  rb_hash_aset(kwargs, ID2SYM(rb_intern("content")), rbs_string_to_ruby_str(input.content));
  VALUE argv[1] = {kwargs};
  VALUE buffer = rb_funcallv_kw(RBS_Buffer, rb_intern("new"), 1, argv, RB_PASS_KEYWORDS);
  return buffer;
}

#define RBS_LOC_CHILDREN_SIZE(cap) (sizeof(rbs_loc_children) + sizeof(rbs_loc_entry) * ((cap) - 1))

VALUE rbs_location_wrap_into_ruby_obj(rbs_location_t *input) {
  return rb_data_typed_object_wrap(RBS_Location, input, &location_type);
}
