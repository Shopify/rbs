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

VALUE rbs_ast_annotation(VALUE string, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("string")), string);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Annotation,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_comment(VALUE string, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("string")), string);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Comment,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_decl_class(rbs_typename_t *name, rbs_node_list_t *type_params, rbs_ast_declarations_class_super_t *super_class, rbs_node_list_t *members, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type_params")), type_params == NULL ? Qnil : type_params->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("super_class")), super_class == NULL ? Qnil : super_class->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("members")), members == NULL ? Qnil : members->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("annotations")), annotations == NULL ? Qnil : annotations->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Declarations_Class,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_decl_class_super(rbs_typename_t *name, rbs_node_list_t *args, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("args")), args == NULL ? Qnil : args->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Declarations_Class_Super,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_decl_class_alias(rbs_typename_t *new_name, rbs_typename_t *old_name, rbs_location_t *location, VALUE comment) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("new_name")), new_name == NULL ? Qnil : new_name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("old_name")), old_name == NULL ? Qnil : old_name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Declarations_ClassAlias,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_decl_constant(rbs_typename_t *name, rbs_node_t *type, rbs_location_t *location, VALUE comment) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type == NULL ? Qnil : type->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Declarations_Constant,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_decl_global(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_location_t *location, VALUE comment) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type == NULL ? Qnil : type->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Declarations_Global,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_decl_interface(rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_list_t *members, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type_params")), type_params == NULL ? Qnil : type_params->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("members")), members == NULL ? Qnil : members->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("annotations")), annotations == NULL ? Qnil : annotations->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Declarations_Interface,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_decl_module(rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_list_t *self_types, rbs_node_list_t *members, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type_params")), type_params == NULL ? Qnil : type_params->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("self_types")), self_types == NULL ? Qnil : self_types->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("members")), members == NULL ? Qnil : members->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("annotations")), annotations == NULL ? Qnil : annotations->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Declarations_Module,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_decl_module_self(rbs_typename_t *name, rbs_node_list_t *args, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("args")), args == NULL ? Qnil : args->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Declarations_Module_Self,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_decl_module_alias(rbs_typename_t *new_name, rbs_typename_t *old_name, rbs_location_t *location, VALUE comment) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("new_name")), new_name == NULL ? Qnil : new_name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("old_name")), old_name == NULL ? Qnil : old_name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Declarations_ModuleAlias,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_decl_type_alias(rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_t *type, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type_params")), type_params == NULL ? Qnil : type_params->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type == NULL ? Qnil : type->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("annotations")), annotations == NULL ? Qnil : annotations->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Declarations_TypeAlias,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_directives_use(rbs_node_list_t *clauses, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("clauses")), clauses == NULL ? Qnil : clauses->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Directives_Use,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_directives_use_single_clause(rbs_typename_t *type_name, rbs_ast_symbol_t *new_name, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type_name")), type_name == NULL ? Qnil : type_name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("new_name")), new_name == NULL ? Qnil : new_name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Directives_Use_SingleClause,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_directives_use_wildcard_clause(rbs_namespace_t *namespace, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("namespace")), namespace == NULL ? Qnil : namespace->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Directives_Use_WildcardClause,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_alias(rbs_ast_symbol_t *new_name, rbs_ast_symbol_t *old_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("new_name")), new_name == NULL ? Qnil : new_name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("old_name")), old_name == NULL ? Qnil : old_name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("kind")), kind == NULL ? Qnil : kind->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("annotations")), annotations == NULL ? Qnil : annotations->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_Alias,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_attr_accessor(rbs_ast_symbol_t *name, rbs_node_t *type, VALUE ivar_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment, rbs_ast_symbol_t *visibility) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type == NULL ? Qnil : type->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("ivar_name")), ivar_name);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("kind")), kind == NULL ? Qnil : kind->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("annotations")), annotations == NULL ? Qnil : annotations->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("visibility")), visibility == NULL ? Qnil : visibility->base.cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_AttrAccessor,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_attr_reader(rbs_ast_symbol_t *name, rbs_node_t *type, VALUE ivar_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment, rbs_ast_symbol_t *visibility) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type == NULL ? Qnil : type->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("ivar_name")), ivar_name);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("kind")), kind == NULL ? Qnil : kind->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("annotations")), annotations == NULL ? Qnil : annotations->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("visibility")), visibility == NULL ? Qnil : visibility->base.cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_AttrReader,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_attr_writer(rbs_ast_symbol_t *name, rbs_node_t *type, VALUE ivar_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment, rbs_ast_symbol_t *visibility) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type == NULL ? Qnil : type->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("ivar_name")), ivar_name);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("kind")), kind == NULL ? Qnil : kind->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("annotations")), annotations == NULL ? Qnil : annotations->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("visibility")), visibility == NULL ? Qnil : visibility->base.cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_AttrWriter,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_class_instance_variable(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_location_t *location, VALUE comment) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type == NULL ? Qnil : type->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_ClassInstanceVariable,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_class_variable(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_location_t *location, VALUE comment) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type == NULL ? Qnil : type->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_ClassVariable,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_extend(rbs_typename_t *name, rbs_node_list_t *args, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("args")), args == NULL ? Qnil : args->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("annotations")), annotations == NULL ? Qnil : annotations->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_Extend,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_include(rbs_typename_t *name, rbs_node_list_t *args, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("args")), args == NULL ? Qnil : args->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("annotations")), annotations == NULL ? Qnil : annotations->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_Include,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_instance_variable(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_location_t *location, VALUE comment) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type == NULL ? Qnil : type->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_InstanceVariable,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_method_definition(rbs_ast_symbol_t *name, rbs_ast_symbol_t *kind, rbs_node_list_t *overloads, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment, bool overloading, rbs_ast_symbol_t *visibility) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("kind")), kind == NULL ? Qnil : kind->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("overloads")), overloads == NULL ? Qnil : overloads->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("annotations")), annotations == NULL ? Qnil : annotations->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("overloading")), overloading ? Qtrue : Qfalse);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("visibility")), visibility == NULL ? Qnil : visibility->base.cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_MethodDefinition,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_method_definition_overload(rbs_node_list_t *annotations, rbs_node_t *method_type) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("annotations")), annotations == NULL ? Qnil : annotations->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("method_type")), method_type == NULL ? Qnil : method_type->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_MethodDefinition_Overload,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_prepend(rbs_typename_t *name, rbs_node_list_t *args, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("args")), args == NULL ? Qnil : args->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("annotations")), annotations == NULL ? Qnil : annotations->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("comment")), comment);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_Prepend,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_private(rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_Private,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_members_public(rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_AST_Members_Public,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_symbol() {
  VALUE _init_kwargs = rb_hash_new();

  return CLASS_NEW_INSTANCE(
    RBS_AST_Symbol,
    1,
    &_init_kwargs
  );
}

VALUE rbs_ast_type_param(rbs_ast_symbol_t *name, rbs_ast_symbol_t *variance, rbs_node_t *upper_bound, rbs_node_t *default_type, bool unchecked, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("variance")), variance == NULL ? Qnil : variance->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("upper_bound")), upper_bound == NULL ? Qnil : upper_bound->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("default_type")), default_type == NULL ? Qnil : default_type->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("unchecked")), unchecked ? Qtrue : Qfalse);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_AST_TypeParam,
    1,
    &_init_kwargs
  );
}

VALUE rbs_method_type(rbs_node_list_t *type_params, rbs_node_t *type, rbs_types_block_t *block, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type_params")), type_params == NULL ? Qnil : type_params->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type == NULL ? Qnil : type->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("block")), block == NULL ? Qnil : block->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_MethodType,
    1,
    &_init_kwargs
  );
}

VALUE rbs_namespace(rbs_node_list_t *path, bool absolute) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("path")), path == NULL ? Qnil : path->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("absolute")), absolute ? Qtrue : Qfalse);

  return CLASS_NEW_INSTANCE(
    RBS_Namespace,
    1,
    &_init_kwargs
  );
}

VALUE rbs_type_name(rbs_namespace_t *namespace, rbs_ast_symbol_t *name) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("namespace")), namespace == NULL ? Qnil : namespace->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_TypeName,
    1,
    &_init_kwargs
  );
}

VALUE rbs_alias(rbs_typename_t *name, rbs_node_list_t *args, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("args")), args == NULL ? Qnil : args->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Alias,
    1,
    &_init_kwargs
  );
}

VALUE rbs_bases_any(VALUE todo, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("todo")), todo);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Bases_Any,
    1,
    &_init_kwargs
  );
}

VALUE rbs_bases_bool(rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Bases_Bool,
    1,
    &_init_kwargs
  );
}

VALUE rbs_bases_bottom(rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Bases_Bottom,
    1,
    &_init_kwargs
  );
}

VALUE rbs_bases_class(rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Bases_Class,
    1,
    &_init_kwargs
  );
}

VALUE rbs_bases_instance(rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Bases_Instance,
    1,
    &_init_kwargs
  );
}

VALUE rbs_bases_nil(rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Bases_Nil,
    1,
    &_init_kwargs
  );
}

VALUE rbs_bases_self(rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Bases_Self,
    1,
    &_init_kwargs
  );
}

VALUE rbs_bases_top(rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Bases_Top,
    1,
    &_init_kwargs
  );
}

VALUE rbs_bases_void(rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Bases_Void,
    1,
    &_init_kwargs
  );
}

VALUE rbs_block(rbs_node_t *type, bool required, rbs_node_t *self_type) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type == NULL ? Qnil : type->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("required")), required ? Qtrue : Qfalse);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("self_type")), self_type == NULL ? Qnil : self_type->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Block,
    1,
    &_init_kwargs
  );
}

VALUE rbs_class_instance(rbs_typename_t *name, rbs_node_list_t *args, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("args")), args == NULL ? Qnil : args->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_ClassInstance,
    1,
    &_init_kwargs
  );
}

VALUE rbs_class_singleton(rbs_typename_t *name, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

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

VALUE rbs_function_param(rbs_node_t *type, rbs_ast_symbol_t *name, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type == NULL ? Qnil : type->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Function_Param,
    1,
    &_init_kwargs
  );
}

VALUE rbs_interface(rbs_typename_t *name, rbs_node_list_t *args, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("args")), args == NULL ? Qnil : args->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Interface,
    1,
    &_init_kwargs
  );
}

VALUE rbs_intersection(rbs_node_list_t *types, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("types")), types == NULL ? Qnil : types->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Intersection,
    1,
    &_init_kwargs
  );
}

VALUE rbs_literal(VALUE literal, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("literal")), literal);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Literal,
    1,
    &_init_kwargs
  );
}

VALUE rbs_optional(rbs_node_t *type, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type == NULL ? Qnil : type->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Optional,
    1,
    &_init_kwargs
  );
}

VALUE rbs_proc(rbs_node_t *type, rbs_types_block_t *block, rbs_location_t *location, rbs_node_t *self_type) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("type")), type == NULL ? Qnil : type->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("block")), block == NULL ? Qnil : block->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("self_type")), self_type == NULL ? Qnil : self_type->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Proc,
    1,
    &_init_kwargs
  );
}

VALUE rbs_record(VALUE all_fields, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("all_fields")), all_fields);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Record,
    1,
    &_init_kwargs
  );
}

VALUE rbs_tuple(rbs_node_list_t *types, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("types")), types == NULL ? Qnil : types->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Tuple,
    1,
    &_init_kwargs
  );
}

VALUE rbs_union(rbs_node_list_t *types, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("types")), types == NULL ? Qnil : types->cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Union,
    1,
    &_init_kwargs
  );
}

VALUE rbs_untyped_function(rbs_node_t *return_type) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("return_type")), return_type == NULL ? Qnil : return_type->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_UntypedFunction,
    1,
    &_init_kwargs
  );
}

VALUE rbs_variable(rbs_ast_symbol_t *name, rbs_location_t *location) {
  VALUE _init_kwargs = rb_hash_new();
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("name")), name == NULL ? Qnil : name->base.cached_ruby_value);
  rb_hash_aset(_init_kwargs, ID2SYM(rb_intern("location")), location == NULL ? Qnil : location->cached_ruby_value);

  return CLASS_NEW_INSTANCE(
    RBS_Types_Variable,
    1,
    &_init_kwargs
  );
}

