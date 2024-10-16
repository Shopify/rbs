#include "temp_utils.h"

#include "constants.h"
#include "ruby/encoding.h"

VALUE rbs_string_to_ruby_str(const rbs_string_t input) {
  return rb_enc_str_new(input.start, rbs_string_len(input), rb_utf8_encoding());
}

rbs_string_t rbs_string_from_ruby_str(const VALUE input) {
  return rbs_string_new(RSTRING_PTR(input), RSTRING_END(input));
}

VALUE rbs_buffer_content_ruby_str(const rbs_buffer_t input) {
  return rbs_string_to_ruby_str(input.content);
}

VALUE rbs_buffer_to_ruby_buffer(const rbs_buffer_t input) {
  VALUE kwargs = rb_hash_new();
  rb_hash_aset(kwargs, ID2SYM(rb_intern("name")), rbs_string_to_ruby_str(input.name));
  rb_hash_aset(kwargs, ID2SYM(rb_intern("content")), rbs_string_to_ruby_str(input.content));
  VALUE argv[1] = {kwargs};
  VALUE buffer = rb_funcallv_kw(RBS_Buffer, rb_intern("new"), 1, argv, RB_PASS_KEYWORDS);
  return buffer;
}
