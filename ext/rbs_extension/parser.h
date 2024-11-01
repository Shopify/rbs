#ifndef RBS__PARSER_H
#define RBS__PARSER_H

#include "ruby.h"
#include "parserstate.h"

/**
 * RBS::Parser class
 * */
extern VALUE RBS_Parser;

rbs_node_t *parse_type(parserstate *state);
rbs_methodtype_t *parse_method_type(parserstate *state);
rbs_node_list_t *parse_signature(parserstate *state);

void rbs__init_parser();

#endif
