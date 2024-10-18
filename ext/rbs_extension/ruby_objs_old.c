#include "rbs_extension.h"

#ifdef RB_PASS_KEYWORDS
  // Ruby 2.7 or later
  #define CLASS_NEW_INSTANCE(klass, argc, argv)\
          rb_class_new_instance_kw(argc, argv, klass, RB_PASS_KEYWORDS)
#else
  // Ruby 2.6
  #define CLASS_NEW_INSTANCE(receiver, argc, argv)\
          rb_class_new_instance(argc, argv, receiver)
#endif

VALUE rbs_base_type(VALUE klass, VALUE location) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    klass,
    1,
    &args
  );
}

VALUE rbs_ast_type_param(VALUE name, VALUE variance, bool unchecked, VALUE upper_bound, VALUE default_type, VALUE location) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("name")), name);
  rb_hash_aset(args, ID2SYM(rb_intern("variance")), variance);
  rb_hash_aset(args, ID2SYM(rb_intern("upper_bound")), upper_bound);
  rb_hash_aset(args, ID2SYM(rb_intern("default_type")), default_type);
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);

  VALUE type_param = CLASS_NEW_INSTANCE(RBS_AST_TypeParam, 1, &args);

  if (unchecked) {
    rb_funcall(type_param, rb_intern("unchecked!"), 0);
  }

  return type_param;
}

VALUE rbs_ast_members_variable(VALUE klass, VALUE name, VALUE type, VALUE location, VALUE comment) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("name")), name);
  rb_hash_aset(args, ID2SYM(rb_intern("type")), type);
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);
  rb_hash_aset(args, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    klass,
    1,
    &args
  );
}

VALUE rbs_ast_members_mixin(VALUE klass, VALUE name, VALUE module_args, VALUE annotations, VALUE location, VALUE comment) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("name")), name);
  rb_hash_aset(args, ID2SYM(rb_intern("args")), module_args);
  rb_hash_aset(args, ID2SYM(rb_intern("annotations")), annotations);
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);
  rb_hash_aset(args, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    klass,
    1,
    &args
  );
}

VALUE rbs_ast_members_attribute(VALUE klass, VALUE name, VALUE type, VALUE ivar_name, VALUE kind, VALUE annotations, VALUE location, VALUE comment, VALUE visibility) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("name")), name);
  rb_hash_aset(args, ID2SYM(rb_intern("type")), type);
  rb_hash_aset(args, ID2SYM(rb_intern("ivar_name")), ivar_name);
  rb_hash_aset(args, ID2SYM(rb_intern("kind")), kind);
  rb_hash_aset(args, ID2SYM(rb_intern("annotations")), annotations);
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);
  rb_hash_aset(args, ID2SYM(rb_intern("comment")), comment);
  rb_hash_aset(args, ID2SYM(rb_intern("visibility")), visibility);

  return CLASS_NEW_INSTANCE(
    klass,
    1,
    &args
  );
}

