/*----------------------------------------------------------------------------*/
/* This file is generated by the templates/template.rb script and should not  */
/* be modified manually.                                                      */
/* To change the template see                                                 */
/* templates/src/ruby_objs.c.erb                                              */
/*----------------------------------------------------------------------------*/

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

VALUE rbs_ast_members_method_definition_overload(VALUE annotations, VALUE method_type) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("annotations")), annotations);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("method_type")), method_type);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_MethodDefinition_Overload,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_prepend(VALUE name, VALUE args, VALUE annotations, VALUE location, VALUE comment) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("args")), args);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("annotations")), annotations);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_Prepend,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_private(VALUE location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_Private,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_public(VALUE location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_Public,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_type_param(VALUE name, VALUE variance, VALUE upper_bound, VALUE default_type, VALUE unchecked, VALUE location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("variance")), variance);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("upper_bound")), upper_bound);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("default_type")), default_type);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("unchecked")), unchecked);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_AST_TypeParam,
    1,
    &_init_kwargs
  );
}

VALUE rbs_method_type(VALUE type_params, VALUE type, VALUE block, VALUE location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type_params")), type_params);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("block")), block);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_MethodType,
    1,
    &_init_kwargs
  );
}

VALUE rbs_namespace(VALUE path, VALUE absolute) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("path")), path);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("absolute")), absolute);

  return CLASS_NEW_INSTANCE(
    RBS_Namespace,
    1,
    &_init_kwargs
  );
}

VALUE rbs_type_name(VALUE namespace, VALUE name) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("namespace")), namespace);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name);

  return CLASS_NEW_INSTANCE(
    RBS_TypeName,
    1,
    &_init_kwargs
  );
}

VALUE rbs_alias(VALUE name, VALUE args, VALUE location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("args")), args);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Alias,
    1,
    &_init_kwargs
  );
}

VALUE rbs_block(VALUE type, VALUE required, VALUE self_type) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("required")), required);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("self_type")), self_type);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Block,
    1,
    &_init_kwargs
  );
}

VALUE rbs_class_instance(VALUE name, VALUE args, VALUE location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("args")), args);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_ClassInstance,
    1,
    &_init_kwargs
  );
}

VALUE rbs_class_singleton(VALUE name, VALUE location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_ClassSingleton,
    1,
    &_init_kwargs
  );
}

VALUE rbs_function(VALUE required_positionals, VALUE optional_positionals, VALUE rest_positionals, VALUE trailing_positionals, VALUE required_keywords, VALUE optional_keywords, VALUE rest_keywords, VALUE return_type) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("required_positionals")), required_positionals);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("optional_positionals")), optional_positionals);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("rest_positionals")), rest_positionals);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("trailing_positionals")), trailing_positionals);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("required_keywords")), required_keywords);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("optional_keywords")), optional_keywords);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("rest_keywords")), rest_keywords);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("return_type")), return_type);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Function,
    1,
    &_init_kwargs
  );
}

VALUE rbs_function_param(VALUE type, VALUE name, VALUE location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Function_Param,
    1,
    &_init_kwargs
  );
}

VALUE rbs_interface(VALUE name, VALUE args, VALUE location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("args")), args);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Interface,
    1,
    &_init_kwargs
  );
}

VALUE rbs_intersection(VALUE types, VALUE location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("types")), types);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Intersection,
    1,
    &_init_kwargs
  );
}

VALUE rbs_literal(VALUE literal, VALUE location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("literal")), literal);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Literal,
    1,
    &_init_kwargs
  );
}

VALUE rbs_proc(VALUE type, VALUE block, VALUE location, VALUE self_type) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("block")), block);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("self_type")), self_type);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Proc,
    1,
    &_init_kwargs
  );
}

VALUE rbs_union(VALUE types, VALUE location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("types")), types);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Union,
    1,
    &_init_kwargs
  );
}

VALUE rbs_untyped_function(VALUE return_type) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("return_type")), return_type);

  return CLASS_NEW_INSTANCE(
    RBS_Types_UntypedFunction,
    1,
    &_init_kwargs
  );
}

