/*----------------------------------------------------------------------------*/
/* This file is generated by the templates/template.rb script and should not  */
/* be modified manually.                                                      */
/* To change the template see                                                 */
/* templates/include/rbs/ruby_objs.h.erb                                      */
/*----------------------------------------------------------------------------*/

#ifndef RBS__RUBY_OBJS_H
#define RBS__RUBY_OBJS_H

#include "ruby.h"

VALUE rbs_ast_members_attr_writer(VALUE name, VALUE type, VALUE ivar_name, VALUE kind, VALUE annotations, VALUE location, VALUE comment, VALUE visibility);
VALUE rbs_ast_members_class_instance_variable(VALUE name, VALUE type, VALUE location, VALUE comment);
VALUE rbs_ast_members_class_variable(VALUE name, VALUE type, VALUE location, VALUE comment);
VALUE rbs_ast_members_extend(VALUE name, VALUE args, VALUE annotations, VALUE location, VALUE comment);
VALUE rbs_ast_members_include(VALUE name, VALUE args, VALUE annotations, VALUE location, VALUE comment);
VALUE rbs_ast_members_instance_variable(VALUE name, VALUE type, VALUE location, VALUE comment);
VALUE rbs_ast_members_method_definition(VALUE name, VALUE kind, VALUE overloads, VALUE annotations, VALUE location, VALUE comment, VALUE overloading, VALUE visibility);
VALUE rbs_ast_members_method_definition_overload(VALUE annotations, VALUE method_type);
VALUE rbs_ast_members_prepend(VALUE name, VALUE args, VALUE annotations, VALUE location, VALUE comment);
VALUE rbs_ast_members_private(VALUE location);
VALUE rbs_ast_members_public(VALUE location);
VALUE rbs_ast_type_param(VALUE name, VALUE variance, VALUE upper_bound, VALUE default_type, VALUE unchecked, VALUE location);
VALUE rbs_method_type(VALUE type_params, VALUE type, VALUE block, VALUE location);
VALUE rbs_namespace(VALUE path, VALUE absolute);
VALUE rbs_type_name(VALUE namespace, VALUE name);
VALUE rbs_alias(VALUE name, VALUE args, VALUE location);
VALUE rbs_block(VALUE type, VALUE required, VALUE self_type);
VALUE rbs_class_instance(VALUE name, VALUE args, VALUE location);
VALUE rbs_class_singleton(VALUE name, VALUE location);
VALUE rbs_function(VALUE required_positionals, VALUE optional_positionals, VALUE rest_positionals, VALUE trailing_positionals, VALUE required_keywords, VALUE optional_keywords, VALUE rest_keywords, VALUE return_type);
VALUE rbs_function_param(VALUE type, VALUE name, VALUE location);
VALUE rbs_interface(VALUE name, VALUE args, VALUE location);
VALUE rbs_intersection(VALUE types, VALUE location);
VALUE rbs_literal(VALUE literal, VALUE location);
VALUE rbs_proc(VALUE type, VALUE block, VALUE location, VALUE self_type);
VALUE rbs_union(VALUE types, VALUE location);
VALUE rbs_untyped_function(VALUE return_type);

#endif
