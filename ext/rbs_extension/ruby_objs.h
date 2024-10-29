#ifndef RBS__RUBY_OBJS_H
#define RBS__RUBY_OBJS_H

#include "ruby.h"

VALUE rbs_alias(VALUE typename, VALUE args, VALUE location);
VALUE rbs_base_type(VALUE klass, VALUE location);
VALUE rbs_block(VALUE type, VALUE required, VALUE self_type);
VALUE rbs_class_instance(VALUE typename, VALUE type_args, VALUE location);
VALUE rbs_class_singleton(VALUE typename, VALUE location);
VALUE rbs_function_param(VALUE type, VALUE name, VALUE location);
VALUE rbs_function(VALUE required_positional_params, VALUE optional_positional_params, VALUE rest_positional_params, VALUE trailing_positional_params, VALUE required_keywords, VALUE optional_keywords, VALUE rest_keywords, VALUE return_type);
VALUE rbs_untyped_function(VALUE return_type);
VALUE rbs_interface(VALUE typename, VALUE type_args, VALUE location);
VALUE rbs_intersection(VALUE types, VALUE location);
VALUE rbs_literal(VALUE literal, VALUE location);
VALUE rbs_namespace(VALUE path, VALUE absolute);
VALUE rbs_optional(VALUE type, VALUE location);
VALUE rbs_proc(VALUE function, VALUE block, VALUE location, VALUE self_type);
VALUE rbs_record(VALUE fields, VALUE location);
VALUE rbs_tuple(VALUE types, VALUE location);
VALUE rbs_type_name(VALUE namespace, VALUE name);
VALUE rbs_union(VALUE types, VALUE location);
VALUE rbs_variable(VALUE name, VALUE location);

#endif
