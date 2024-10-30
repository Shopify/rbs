#ifndef RBS__PARSER_H
#define RBS__PARSER_H

#include "ruby.h"
#include "parserstate.h"

/**
 * RBS::Parser class
 * */
extern VALUE RBS_Parser;

VALUE rbs_ast_new(rbs_node_t *node);

VALUE parse_type(parserstate *state);
rbs_methodtype_t *parse_method_type(parserstate *state);
VALUE parse_signature(parserstate *state);

void rbs__init_parser();

#endif
