/*----------------------------------------------------------------------------*/
/* This file is generated by the templates/template.rb script and should not  */
/* be modified manually.                                                      */
/* To change the template see                                                 */
/* templates/ext/rbs_extension/ast_translation.h.erb                          */
/*----------------------------------------------------------------------------*/

#ifndef RBS_EXTENSION_AST_TRANSLATION_H
#define RBS_EXTENSION_AST_TRANSLATION_H

#include "ruby.h"
#include "rbs/ast.h"

/*
 * Convert a `rbs_node_t*` struct to a Ruby VALUE instance.
 *
 * Warning: the C struct is freed once the VALUE instance is created, do not use it after this point.
 */
VALUE rbs_struct_to_ruby_value(rbs_node_t *node);

#endif
