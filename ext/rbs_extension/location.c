#include "rbs_extension.h"
#include "location.h"
#include "temp_utils.h"

#define RBS_LOC_REQUIRED_P(loc, i) ((loc)->children->required_p & (1 << (i)))
#define RBS_LOC_OPTIONAL_P(loc, i) (!RBS_LOC_REQUIRED_P((loc), (i)))
#define RBS_LOC_CHILDREN_SIZE(cap) (sizeof(rbs_loc_children) + sizeof(rbs_loc_entry) * ((cap) - 1))
#define NULL_LOC_RANGE_P(rg) ((rg).start == -1)

rbs_loc_range RBS_LOC_NULL_RANGE = { -1, -1 };
VALUE RBS_Location;

position rbs_loc_position(int char_pos) {
  position pos = { 0, char_pos, -1, -1 };
  return pos;
}

position rbs_loc_position3(int char_pos, int line, int column) {
  position pos = { 0, char_pos, line, column };
  return pos;
}

rbs_loc_range rbs_new_loc_range(range rg) {
  rbs_loc_range r = { rg.start.char_pos, rg.end.char_pos };
  return r;
}

// void rbs_loc_init(rbs_loc *loc, VALUE buffer, rbs_loc_range rg) {
//   loc->buffer = buffer;
//   loc->rg = rg;
//   loc->children = NULL;
// }

void rbs_loc_free(rbs_location_t *loc) {
  // free(loc->children);
  // ruby_xfree(loc);
}

static void rbs_loc_mark(void *ptr) { /* no-op */ }

static size_t rbs_loc_memsize(const void *ptr) {
  const rbs_location_t *loc = ptr;
  if (loc->children == NULL) {
    return sizeof(rbs_location_t);
  } else {
    return sizeof(rbs_location_t) + RBS_LOC_CHILDREN_SIZE(loc->children->cap);
  }
}

const rb_data_type_t location_type = {
  "RBS::Location",
  {rbs_loc_mark, (RUBY_DATA_FUNC)rbs_loc_free, rbs_loc_memsize},
  0, 0, 0 // RUBY_TYPED_FREE_IMMEDIATELY
};

static VALUE location_s_allocate(VALUE klass) {
  rbs_location_t *loc;
  VALUE obj = TypedData_Make_Struct(klass, rbs_location_t, &location_type, loc);

  rbs_location_init(loc, RBS_BUFFER_NULL, RBS_LOC_NULL_RANGE);

  return obj;
}

rbs_location_t *rbs_check_location(VALUE obj) {
  return rb_check_typeddata(obj, &location_type);
}

static VALUE location_initialize(VALUE self, VALUE buffer, VALUE start_pos, VALUE end_pos) {
  rbs_location_t *loc = rbs_check_location(self);

  int start = FIX2INT(start_pos);
  int end = FIX2INT(end_pos);

  loc->buffer = rbs_buffer_copy_from_ruby_buffer(buffer);
  loc->range.start = start;
  loc->range.end = end;

  return Qnil;
}

static VALUE location_initialize_copy(VALUE self, VALUE other) {
  rbs_location_t *self_loc = rbs_check_location(self);
  rbs_location_t *other_loc = rbs_check_location(other);

  self_loc->buffer = other_loc->buffer;
  self_loc->range = other_loc->range;
  if (other_loc->children != NULL) {
    rbs_location_alloc_children(self_loc, other_loc->children->cap);
    memcpy(self_loc->children, other_loc->children, RBS_LOC_CHILDREN_SIZE(other_loc->children->cap));
  }

  return Qnil;
}

static VALUE location_buffer(VALUE self) {
  rbs_location_t *loc = rbs_check_location(self);
  return rbs_buffer_copy_into_ruby_buffer(loc->buffer);
}

static VALUE location_start_pos(VALUE self) {
  rbs_location_t *loc = rbs_check_location(self);
  return INT2FIX(loc->range.start);
}

static VALUE location_end_pos(VALUE self) {
  rbs_location_t *loc = rbs_check_location(self);
  return INT2FIX(loc->range.end);
}

static VALUE location_add_required_child(VALUE self, VALUE name, VALUE start, VALUE end) {
  rbs_location_t *loc = rbs_check_location(self);

  range rg;
  rg.start = rbs_loc_position(FIX2INT(start));
  rg.end = rbs_loc_position(FIX2INT(end));

  rbs_loc_add_required_child(loc, SYM2ID(name), rg);

  return Qnil;
}

static VALUE location_add_optional_child(VALUE self, VALUE name, VALUE start, VALUE end) {
  rbs_location_t *loc = rbs_check_location(self);

  range rg;
  rg.start = rbs_loc_position(FIX2INT(start));
  rg.end = rbs_loc_position(FIX2INT(end));

  rbs_loc_add_optional_child(loc, SYM2ID(name), rg);

  return Qnil;
}

static VALUE location_add_optional_no_child(VALUE self, VALUE name) {
  rbs_location_t *loc = rbs_check_location(self);

  rbs_loc_add_optional_child(loc, SYM2ID(name), NULL_RANGE);

  return Qnil;
}

static VALUE location_aref(VALUE self, VALUE name) {
  rbs_location_t *loc = rbs_check_location(self);

  ID id = SYM2ID(name);

  if (loc->children != NULL) {
    for (unsigned short i = 0; i < loc->children->len; i++) {
      if (loc->children->entries[i].name == id) {
        rbs_loc_range result = loc->children->entries[i].rg;

        if (RBS_LOC_OPTIONAL_P(loc, i) && NULL_LOC_RANGE_P(result)) {
          return Qnil;
        } else {
          rbs_location_t *new_loc = calloc(1, sizeof(rbs_location_t));
          *new_loc = rbs_location_new(loc->buffer, result);

          return rbs_location_wrap_into_ruby_obj(new_loc);
        }
      }
    }
  }

  VALUE string = rb_funcall(name, rb_intern("to_s"), 0);
  rb_raise(rb_eRuntimeError, "Unknown child name given: %s", RSTRING_PTR(string));
}

static VALUE location_optional_keys(VALUE self) {
  VALUE keys = rb_ary_new();

  rbs_location_t *loc = rbs_check_location(self);
  rbs_loc_children *children = loc->children;
  if (children == NULL) {
    return keys;
  }

  for (unsigned short i = 0; i < children->len; i++) {
    if (RBS_LOC_OPTIONAL_P(loc, i)) {
      rb_ary_push(keys, ID2SYM(children->entries[i].name));

    }
  }

  return keys;
}

static VALUE location_required_keys(VALUE self) {
  VALUE keys = rb_ary_new();

  rbs_location_t *loc = rbs_check_location(self);
  rbs_loc_children *children = loc->children;
  if (children == NULL) {
    return keys;
  }

  for (unsigned short i = 0; i < children->len; i++) {
    if (RBS_LOC_REQUIRED_P(loc, i)) {
      rb_ary_push(keys, ID2SYM(children->entries[i].name));
    }
  }

  return keys;
}

void rbs__init_location(void) {
  RBS_Location = rb_define_class_under(RBS, "Location", rb_cObject);
  rb_define_alloc_func(RBS_Location, location_s_allocate);
  rb_define_private_method(RBS_Location, "initialize", location_initialize, 3);
  rb_define_private_method(RBS_Location, "initialize_copy", location_initialize_copy, 1);
  rb_define_method(RBS_Location, "buffer", location_buffer, 0);
  rb_define_method(RBS_Location, "start_pos", location_start_pos, 0);
  rb_define_method(RBS_Location, "end_pos", location_end_pos, 0);
  rb_define_method(RBS_Location, "_add_required_child", location_add_required_child, 3);
  rb_define_method(RBS_Location, "_add_optional_child", location_add_optional_child, 3);
  rb_define_method(RBS_Location, "_add_optional_no_child", location_add_optional_no_child, 1);
  rb_define_method(RBS_Location, "_optional_keys", location_optional_keys, 0);
  rb_define_method(RBS_Location, "_required_keys", location_required_keys, 0);
  rb_define_method(RBS_Location, "[]", location_aref, 1);
}
