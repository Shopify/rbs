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

