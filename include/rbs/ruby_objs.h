/*----------------------------------------------------------------------------*/
/* This file is generated by the templates/template.rb script and should not  */
/* be modified manually.                                                      */
/* To change the template see                                                 */
/* templates/include/rbs/ruby_objs.h.erb                                      */
/*----------------------------------------------------------------------------*/

#ifndef RBS__RUBY_OBJS_H
#define RBS__RUBY_OBJS_H

#include "ruby.h"

VALUE rbs_intersection(VALUE types, VALUE location);
VALUE rbs_literal(VALUE literal, VALUE location);
VALUE rbs_proc(VALUE type, VALUE block, VALUE location, VALUE self_type);
VALUE rbs_union(VALUE types, VALUE location);
VALUE rbs_untyped_function(VALUE return_type);

#endif
