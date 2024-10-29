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

VALUE rbs_class_instance(VALUE typename, VALUE type_args, VALUE location) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("name")), typename);
  rb_hash_aset(args, ID2SYM(rb_intern("args")), type_args);
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_ClassInstance,
    1,
    &args
  );
}

VALUE rbs_class_singleton(VALUE typename, VALUE location) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("name")), typename);
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_ClassSingleton,
    1,
    &args
  );
}

VALUE rbs_alias(VALUE typename, VALUE args, VALUE location) {
  VALUE kwargs = rb_hash_new();
  rb_hash_aset(kwargs, ID2SYM(rb_intern("name")), typename);
  rb_hash_aset(kwargs, ID2SYM(rb_intern("args")), args);
  rb_hash_aset(kwargs, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Alias,
    1,
    &kwargs
  );
}

VALUE rbs_interface(VALUE typename, VALUE type_args, VALUE location) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("name")), typename);
  rb_hash_aset(args, ID2SYM(rb_intern("args")), type_args);
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Interface,
    1,
    &args
  );
}

VALUE rbs_union(VALUE types, VALUE location) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("types")), types);
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Union,
    1,
    &args
  );
}

VALUE rbs_intersection(VALUE types, VALUE location) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("types")), types);
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Intersection,
    1,
    &args
  );
}

VALUE rbs_tuple(VALUE types, VALUE location) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("types")), types);
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Tuple,
    1,
    &args
  );
}

VALUE rbs_optional(VALUE type, VALUE location) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("type")), type);
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Optional,
    1,
    &args
  );
}

VALUE rbs_block(VALUE type, VALUE required, VALUE self_type) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("type")), type);
  rb_hash_aset(args, ID2SYM(rb_intern("required")), required);
  rb_hash_aset(args, ID2SYM(rb_intern("self_type")), self_type);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Block,
    1,
    &args
  );
}

VALUE rbs_function_param(VALUE type, VALUE name, VALUE location) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("type")), type);
  rb_hash_aset(args, ID2SYM(rb_intern("name")), name);
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Function_Param,
    1,
    &args
  );
}

VALUE rbs_untyped_function(VALUE return_type) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("return_type")), return_type);

  return CLASS_NEW_INSTANCE(
    RBS_Types_UntypedFunction,
    1,
    &args
  );
}

VALUE rbs_function(
  VALUE required_positional_params,
  VALUE optional_positional_params,
  VALUE rest_positional_param,
  VALUE trailing_positional_params,
  VALUE required_keyword_params,
  VALUE optional_keyword_params,
  VALUE rest_keyword_param,
  VALUE return_type
) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("required_positionals")), required_positional_params);
  rb_hash_aset(args, ID2SYM(rb_intern("optional_positionals")), optional_positional_params);
  rb_hash_aset(args, ID2SYM(rb_intern("rest_positionals")), rest_positional_param);
  rb_hash_aset(args, ID2SYM(rb_intern("trailing_positionals")), trailing_positional_params);
  rb_hash_aset(args, ID2SYM(rb_intern("required_keywords")), required_keyword_params);
  rb_hash_aset(args, ID2SYM(rb_intern("optional_keywords")), optional_keyword_params);
  rb_hash_aset(args, ID2SYM(rb_intern("rest_keywords")), rest_keyword_param);
  rb_hash_aset(args, ID2SYM(rb_intern("return_type")), return_type);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Function,
    1,
    &args
  );
}

VALUE rbs_proc(VALUE function, VALUE block, VALUE location, VALUE self_type) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("type")), function);
  rb_hash_aset(args, ID2SYM(rb_intern("block")), block);
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);
  rb_hash_aset(args, ID2SYM(rb_intern("self_type")), self_type);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Proc,
    1,
    &args
  );
}

VALUE rbs_void(VALUE location) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Bases_Void,
    1,
    &args
  );
}

VALUE rbs_literal(VALUE literal, VALUE location) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);
  rb_hash_aset(args, ID2SYM(rb_intern("literal")), literal);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Literal,
    1,
    &args
  );
}

VALUE rbs_record(VALUE fields,VALUE location) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);
  rb_hash_aset(args, ID2SYM(rb_intern("all_fields")), fields);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Record,
    1,
    &args
  );
}

VALUE rbs_variable(VALUE name, VALUE location) {
  VALUE args = rb_hash_new();
  rb_hash_aset(args, ID2SYM(rb_intern("location")), location);
  rb_hash_aset(args, ID2SYM(rb_intern("name")), name);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Variable,
    1,
    &args
  );
}
