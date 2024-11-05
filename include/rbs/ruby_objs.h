/*----------------------------------------------------------------------------*/
/* This file is generated by the templates/template.rb script and should not  */
/* be modified manually.                                                      */
/* To change the template see                                                 */
/* templates/include/rbs/ruby_objs.h.erb                                      */
/*----------------------------------------------------------------------------*/

#ifndef RBS__RUBY_OBJS_H
#define RBS__RUBY_OBJS_H

#include "ruby.h"

VALUE rbs_ast_annotation(VALUE string, rbs_location_t *location);
VALUE rbs_ast_comment(VALUE string, rbs_location_t *location);
VALUE rbs_ast_decl_class(rbs_typename_t *name, rbs_node_list_t *type_params, VALUE super_class, rbs_node_list_t *members, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_decl_class_super(VALUE name, rbs_node_list_t *args, rbs_location_t *location);
VALUE rbs_ast_decl_class_alias(rbs_typename_t *new_name, rbs_typename_t *old_name, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_decl_constant(rbs_typename_t *name, rbs_node_t *type, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_decl_global(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_decl_interface(rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_list_t *members, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_decl_module(rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_list_t *self_types, rbs_node_list_t *members, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_decl_module_self(rbs_typename_t *name, rbs_node_list_t *args, rbs_location_t *location);
VALUE rbs_ast_decl_module_alias(rbs_typename_t *new_name, rbs_typename_t *old_name, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_decl_type_alias(rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_t *type, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_directives_use(rbs_node_list_t *clauses, rbs_location_t *location);
VALUE rbs_ast_directives_use_single_clause(VALUE type_name, VALUE new_name, rbs_location_t *location);
VALUE rbs_ast_directives_use_wildcard_clause(rbs_namespace_t *namespace, rbs_location_t *location);
VALUE rbs_ast_members_alias(rbs_ast_symbol_t *new_name, rbs_ast_symbol_t *old_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_members_attr_accessor(rbs_ast_symbol_t *name, rbs_node_t *type, VALUE ivar_name, VALUE kind, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment, VALUE visibility);
VALUE rbs_ast_members_attr_reader(rbs_ast_symbol_t *name, rbs_node_t *type, VALUE ivar_name, VALUE kind, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment, VALUE visibility);
VALUE rbs_ast_members_attr_writer(rbs_ast_symbol_t *name, rbs_node_t *type, VALUE ivar_name, VALUE kind, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment, VALUE visibility);
VALUE rbs_ast_members_class_instance_variable(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_members_class_variable(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_members_extend(VALUE name, rbs_node_list_t *args, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_members_include(VALUE name, rbs_node_list_t *args, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_members_instance_variable(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_members_method_definition(rbs_ast_symbol_t *name, rbs_ast_symbol_t *kind, VALUE overloads, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment, VALUE overloading, VALUE visibility);
VALUE rbs_ast_members_method_definition_overload(VALUE annotations, VALUE method_type);
VALUE rbs_ast_members_prepend(VALUE name, rbs_node_list_t *args, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_members_private(rbs_location_t *location);
VALUE rbs_ast_members_public(rbs_location_t *location);
VALUE rbs_ast_symbol();
VALUE rbs_ast_type_param(rbs_ast_symbol_t *name, rbs_ast_symbol_t *variance, VALUE upper_bound, VALUE default_type, rbs_location_t *location);
VALUE rbs_method_type(rbs_node_list_t *type_params, VALUE type, VALUE block, VALUE location);
VALUE rbs_namespace(rbs_node_list_t *path, VALUE absolute);
VALUE rbs_type_name(VALUE namespace, VALUE name);
VALUE rbs_alias(rbs_typename_t *name, rbs_node_list_t *args, VALUE location);
VALUE rbs_bases_any(VALUE location);
VALUE rbs_bases_bool(VALUE location);
VALUE rbs_bases_bottom(VALUE location);
VALUE rbs_bases_class(VALUE location);
VALUE rbs_bases_instance(VALUE location);
VALUE rbs_bases_nil(VALUE location);
VALUE rbs_bases_self(VALUE location);
VALUE rbs_bases_top(VALUE location);
VALUE rbs_bases_void(VALUE location);
VALUE rbs_block(VALUE type, VALUE required, VALUE self_type);
VALUE rbs_class_instance(rbs_typename_t *name, rbs_node_list_t *args, VALUE location);
VALUE rbs_class_singleton(rbs_typename_t *name, VALUE location);
VALUE rbs_function(VALUE required_positionals, VALUE optional_positionals, VALUE rest_positionals, VALUE trailing_positionals, VALUE required_keywords, VALUE optional_keywords, VALUE rest_keywords, VALUE return_type);
VALUE rbs_function_param(rbs_node_t *type, VALUE name, rbs_location_t *location);
VALUE rbs_interface(rbs_typename_t *name, rbs_node_list_t *args, VALUE location);
VALUE rbs_intersection(rbs_node_list_t *types, VALUE location);
VALUE rbs_literal(VALUE literal, VALUE location);
VALUE rbs_optional(rbs_node_t *type, VALUE location);
VALUE rbs_proc(VALUE type, VALUE block, VALUE location, VALUE self_type);
VALUE rbs_record(VALUE all_fields, VALUE location);
VALUE rbs_tuple(rbs_node_list_t *types, VALUE location);
VALUE rbs_union(rbs_node_list_t *types, VALUE location);
VALUE rbs_untyped_function(VALUE return_type);
VALUE rbs_variable(VALUE name, VALUE location);

#endif
