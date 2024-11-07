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
VALUE rbs_ast_decl_class_super(rbs_typename_t *name, rbs_node_list_t *args, rbs_location_t *location);
VALUE rbs_ast_decl_class_alias(rbs_typename_t *new_name, rbs_typename_t *old_name, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_decl_constant(rbs_typename_t *name, rbs_node_t *type, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_decl_global(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_decl_interface(rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_list_t *members, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_decl_module(rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_list_t *self_types, rbs_node_list_t *members, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_decl_module_self(rbs_typename_t *name, rbs_node_list_t *args, rbs_location_t *location);
VALUE rbs_ast_decl_module_alias(rbs_typename_t *new_name, rbs_typename_t *old_name, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_decl_type_alias(rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_t *type, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_directives_use(rbs_node_list_t *clauses, rbs_location_t *location);
VALUE rbs_ast_directives_use_single_clause(rbs_typename_t *type_name, rbs_ast_symbol_t *new_name, rbs_location_t *location);
VALUE rbs_ast_directives_use_wildcard_clause(rbs_namespace_t *namespace, rbs_location_t *location);
VALUE rbs_ast_members_alias(rbs_ast_symbol_t *new_name, rbs_ast_symbol_t *old_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_members_attr_accessor(rbs_ast_symbol_t *name, rbs_node_t *type, VALUE ivar_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment, rbs_ast_symbol_t *visibility);
VALUE rbs_ast_members_attr_reader(rbs_ast_symbol_t *name, rbs_node_t *type, VALUE ivar_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment, rbs_ast_symbol_t *visibility);
VALUE rbs_ast_members_attr_writer(rbs_ast_symbol_t *name, rbs_node_t *type, VALUE ivar_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment, rbs_ast_symbol_t *visibility);
VALUE rbs_ast_members_class_instance_variable(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_members_class_variable(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_members_extend(rbs_typename_t *name, rbs_node_list_t *args, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_members_include(rbs_typename_t *name, rbs_node_list_t *args, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_members_instance_variable(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_members_method_definition(rbs_ast_symbol_t *name, rbs_ast_symbol_t *kind, rbs_node_list_t *overloads, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment, bool overloading, rbs_ast_symbol_t *visibility);
VALUE rbs_ast_members_method_definition_overload(rbs_node_list_t *annotations, rbs_node_t *method_type);
VALUE rbs_ast_members_prepend(rbs_typename_t *name, rbs_node_list_t *args, rbs_node_list_t *annotations, rbs_location_t *location, VALUE comment);
VALUE rbs_ast_members_private(rbs_location_t *location);
VALUE rbs_ast_members_public(rbs_location_t *location);
VALUE rbs_ast_symbol();
VALUE rbs_ast_type_param(rbs_ast_symbol_t *name, rbs_ast_symbol_t *variance, rbs_node_t *upper_bound, rbs_node_t *default_type, bool unchecked, rbs_location_t *location);
VALUE rbs_method_type(rbs_node_list_t *type_params, rbs_node_t *type, rbs_types_block_t *block, rbs_location_t *location);
VALUE rbs_namespace(rbs_node_list_t *path, bool absolute);
VALUE rbs_type_name(rbs_namespace_t *namespace, rbs_ast_symbol_t *name);
VALUE rbs_alias(rbs_typename_t *name, rbs_node_list_t *args, rbs_location_t *location);
VALUE rbs_bases_any(VALUE todo, rbs_location_t *location);
VALUE rbs_bases_bool(rbs_location_t *location);
VALUE rbs_bases_bottom(rbs_location_t *location);
VALUE rbs_bases_class(rbs_location_t *location);
VALUE rbs_bases_instance(rbs_location_t *location);
VALUE rbs_bases_nil(rbs_location_t *location);
VALUE rbs_bases_self(rbs_location_t *location);
VALUE rbs_bases_top(rbs_location_t *location);
VALUE rbs_bases_void(rbs_location_t *location);
VALUE rbs_block(rbs_node_t *type, bool required, rbs_node_t *self_type);
VALUE rbs_class_instance(rbs_typename_t *name, rbs_node_list_t *args, rbs_location_t *location);
VALUE rbs_class_singleton(rbs_typename_t *name, rbs_location_t *location);
VALUE rbs_function(VALUE required_positionals, VALUE optional_positionals, VALUE rest_positionals, VALUE trailing_positionals, VALUE required_keywords, VALUE optional_keywords, VALUE rest_keywords, VALUE return_type);
VALUE rbs_function_param(rbs_node_t *type, rbs_ast_symbol_t *name, rbs_location_t *location);
VALUE rbs_interface(rbs_typename_t *name, rbs_node_list_t *args, rbs_location_t *location);
VALUE rbs_intersection(rbs_node_list_t *types, rbs_location_t *location);
VALUE rbs_literal(VALUE literal, rbs_location_t *location);
VALUE rbs_optional(rbs_node_t *type, rbs_location_t *location);
VALUE rbs_proc(rbs_node_t *type, rbs_types_block_t *block, rbs_location_t *location, rbs_node_t *self_type);
VALUE rbs_record(VALUE all_fields, rbs_location_t *location);
VALUE rbs_tuple(rbs_node_list_t *types, rbs_location_t *location);
VALUE rbs_union(rbs_node_list_t *types, rbs_location_t *location);
VALUE rbs_untyped_function(rbs_node_t *return_type);
VALUE rbs_variable(rbs_ast_symbol_t *name, rbs_location_t *location);

#endif
