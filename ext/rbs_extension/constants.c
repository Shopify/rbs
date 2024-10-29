#include "rbs_extension.h"

#define IMPORT_CONSTANT(var, parent, name) { var = rb_const_get(parent, rb_intern(name)); rb_gc_register_mark_object(var); }

void rbs__init_constants(void) {
}
