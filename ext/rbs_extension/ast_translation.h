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

VALUE rbs_node_list_to_ruby_array(rbs_node_list_t *list);
VALUE rbs_hash_to_ruby_hash(rbs_hash_t *hash);
VALUE rbs_loc_to_ruby_location(rbs_location_t *loc);
VALUE rbs_struct_to_ruby_value(rbs_node_t *instance);

#endif
