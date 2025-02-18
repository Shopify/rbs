/*----------------------------------------------------------------------------*/
/* This file is generated by the templates/template.rb script and should not  */
/* be modified manually.                                                      */
/* To change the template see                                                 */
/* templates/include/rbs/ruby_objs.h.erb                                      */
/*----------------------------------------------------------------------------*/

#ifndef RBS__RUBY_OBJS_H
#define RBS__RUBY_OBJS_H

#include "ruby.h"

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
