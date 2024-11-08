#include "rbs_string_bridging.h"
#include "ruby/encoding.h"

rbs_string_t rbs_string_from_ruby_string(VALUE ruby_string) {
    rb_gc_register_mark_object(ruby_string);

    rbs_string_t s = rbs_string_shared_new(StringValueCStr(ruby_string), RSTRING_END(ruby_string));
    s.cached_ruby_string = ruby_string;
    s.encoding = rb_enc_get(ruby_string);

    return s;
}

VALUE rbs_string_to_ruby_string(rbs_string_t *self) {
    if (self->cached_ruby_string == Qnil) {
        self->cached_ruby_string = rb_str_new_static(self->start, rbs_string_len(*self));
        rb_gc_register_mark_object(self->cached_ruby_string);
    }
    return self->cached_ruby_string;
}
