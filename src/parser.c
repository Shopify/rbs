#include "rbs/parser.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "rbs/defines.h"
#include "rbs/encoding.h"
#include "rbs/rbs_string.h"
#include "rbs/rbs_buffer.h"
#include "rbs/rbs_unescape.h"
#include "rbs/util/rbs_assert.h"

#define INTERN(str)                  \
  rbs_constant_pool_insert_constant( \
    RBS_GLOBAL_CONSTANT_POOL,        \
    (const uint8_t *) str,           \
    strlen(str)                      \
  )

#define INTERN_TOKEN(parserstate, tok)                       \
  rbs_constant_pool_insert_shared_with_encoding(             \
    &parserstate->constant_pool,                             \
    (const uint8_t *) rbs_peek_token(parserstate->lexstate, tok),\
    rbs_token_bytes(tok),                                        \
    (void *) parserstate->lexstate->encoding                 \
  )

#define KEYWORD_CASES \
  case kBOOL:\
  case kBOT: \
  case kCLASS: \
  case kFALSE: \
  case kINSTANCE: \
  case kINTERFACE: \
  case kNIL: \
  case kSELF: \
  case kSINGLETON: \
  case kTOP: \
  case kTRUE: \
  case kVOID: \
  case kTYPE: \
  case kUNCHECKED: \
  case kIN: \
  case kOUT: \
  case kEND: \
  case kDEF: \
  case kINCLUDE: \
  case kEXTEND: \
  case kPREPEND: \
  case kALIAS: \
  case kMODULE: \
  case kATTRREADER: \
  case kATTRWRITER: \
  case kATTRACCESSOR: \
  case kPUBLIC: \
  case kPRIVATE: \
  case kUNTYPED: \
  case kUSE: \
  case kAS: \
  case k__TODO__: \
  /* nop */

#define CHECK_PARSE(call) \
  if (!call) { \
    return false; \
  }

#define ASSERT_TOKEN(state, expected_type) \
  if (state->current_token.type != expected_type) { \
    set_error(state, state->current_token, true, "expected a token `%s`", token_type_str(expected_type)); \
    return false; \
  }

#define ADVANCE_ASSERT(state, expected_type) do {\
    parser_advance(state); \
    ASSERT_TOKEN(state, expected_type) \
  } while(0);

typedef struct {
  rbs_node_list_t *required_positionals;
  rbs_node_list_t *optional_positionals;
  rbs_node_t *rest_positionals;
  rbs_node_list_t *trailing_positionals;
  rbs_hash_t *required_keywords;
  rbs_hash_t *optional_keywords;
  rbs_node_t *rest_keywords;
} method_params;

static bool rbs_is_untyped_params(method_params *params) {
  return params->required_positionals == NULL;
}

/**
 * Returns RBS::Location object of `current_token` of a parser state.
 *
 * @param state
 * @return New RBS::Location object.
 * */
static rbs_location_t *rbs_location_current_token(parserstate *state) {
  return rbs_location_new(&state->allocator, state->current_token.range);
}

static bool parse_optional(parserstate *state, rbs_node_t **optional);
static bool parse_simple(parserstate *state, rbs_node_t **type);

/**
 * @returns A borrowed copy of the current token, which does *not* need to be freed.
 */
static rbs_string_t rbs_parser_peek_current_token(parserstate *state) {
  range rg = state->current_token.range;

  const char *start = state->lexstate->string.start + rg.start.byte_pos;
  size_t length = rg.end.byte_pos - rg.start.byte_pos;

  return rbs_string_new(start, start + length);
}

static rbs_constant_id_t rbs_constant_pool_insert_string(rbs_constant_pool_t *self, rbs_string_t string) {
  return rbs_constant_pool_insert_shared(self, (const uint8_t *) string.start, rbs_string_len(string));
}

typedef enum {
  CLASS_NAME = 1,
  INTERFACE_NAME = 2,
  ALIAS_NAME = 4
} TypeNameKind;

static void parser_advance_no_gap(parserstate *state) {
  if (state->current_token.range.end.byte_pos == state->next_token.range.start.byte_pos) {
    parser_advance(state);
  } else {
    set_error(state, state->next_token, true, "unexpected token");
  }
}

/*
  type_name ::= {`::`} (tUIDENT `::`)* <tXIDENT>
              | {(tUIDENT `::`)*} <tXIDENT>
              | {<tXIDENT>}
*/
NODISCARD
static bool parse_type_name(parserstate *state, TypeNameKind kind, range *rg, rbs_typename_t **typename) {
  bool absolute = false;

  if (rg) {
    rg->start = state->current_token.range.start;
  }

  if (state->current_token.type == pCOLON2) {
    absolute = true;
    parser_advance_no_gap(state);
  }

  rbs_node_list_t *path = rbs_node_list_new(&state->allocator);

  while (
    state->current_token.type == tUIDENT
    && state->next_token.type == pCOLON2
    && state->current_token.range.end.byte_pos == state->next_token.range.start.byte_pos
    && state->next_token.range.end.byte_pos == state->next_token2.range.start.byte_pos
  ) {
    rbs_constant_id_t symbol_value = INTERN_TOKEN(state, state->current_token);
    rbs_location_t *symbolLoc = rbs_location_new(&state->allocator, state->next_token.range);
    rbs_ast_symbol_t *symbol = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, symbol_value);
    rbs_node_list_append(path, (rbs_node_t *)symbol);

    parser_advance(state);
    parser_advance(state);
  }

  range namespace_range = {
    .start = rg->start,
    .end = state->current_token.range.end
  };
  rbs_location_t *loc = rbs_location_new(&state->allocator, namespace_range);
  rbs_namespace_t *namespace = rbs_namespace_new(&state->allocator, loc, path, absolute);

  switch (state->current_token.type) {
    case tLIDENT:
      if (kind & ALIAS_NAME) goto success;
      goto error_handling;
    case tULIDENT:
      if (kind & INTERFACE_NAME) goto success;
      goto error_handling;
    case tUIDENT:
      if (kind & CLASS_NAME) goto success;
      goto error_handling;
    default:
      goto error_handling;
  }

  success: {
    if (rg) {
      rg->end = state->current_token.range.end;
    }

    rbs_location_t *symbolLoc = rbs_location_current_token(state);
    rbs_constant_id_t name = INTERN_TOKEN(state, state->current_token);
    rbs_ast_symbol_t *symbol = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, name);
    *typename = rbs_typename_new(&state->allocator, rbs_location_new(&state->allocator, *rg), namespace, symbol);
    return true;
  }

  error_handling: {
    const char *ids = NULL;
    if (kind & ALIAS_NAME) {
      ids = "alias name";
    }
    if (kind & INTERFACE_NAME) {
      ids = "interface name";
    }
    if (kind & CLASS_NAME) {
      ids = "class/module/constant name";
    }

    rbs_assert(ids != NULL, "Unknown kind of type: %i", kind);

    set_error(state, state->current_token, true, "expected one of %s", ids);
    return false;
  }
}

/*
  type_list ::= {} type `,` ... <`,`> eol
              | {} type `,` ... `,` <type> eol
*/
NODISCARD
static bool parse_type_list(parserstate *state, enum RBSTokenType eol, rbs_node_list_t *types) {
  while (true) {
    rbs_node_t *type;
    CHECK_PARSE(parse_type(state, &type));
    rbs_node_list_append(types, type);

    if (state->next_token.type == pCOMMA) {
      parser_advance(state);

      if (state->next_token.type == eol) {
        break;
      }
    } else {
      if (state->next_token.type == eol) {
        break;
      } else {
        set_error(state, state->next_token, true, "comma delimited type list is expected");
        return false;
      }
    }
  }

  return true;
}

static bool is_keyword_token(enum RBSTokenType type) {
  switch (type)
  {
  case tLIDENT:
  case tUIDENT:
  case tULIDENT:
  case tULLIDENT:
  case tQIDENT:
  case tBANGIDENT:
  KEYWORD_CASES
    return true;
  default:
    return false;
  }
}

/*
  function_param ::= {} <type>
                   | {} type <param>
*/
NODISCARD
static bool parse_function_param(parserstate *state, rbs_types_function_param_t **function_param) {
  range type_range;
  type_range.start = state->next_token.range.start;
  rbs_node_t *type;
  CHECK_PARSE(parse_type(state, &type));
  type_range.end = state->current_token.range.end;

  if (state->next_token.type == pCOMMA || state->next_token.type == pRPAREN) {
    range param_range = type_range;

    rbs_location_t *loc = rbs_location_new(&state->allocator, param_range);
    rbs_loc_alloc_children(&state->allocator, loc, 1);
    rbs_loc_add_optional_child(loc, INTERN("name"), NULL_RANGE);

    *function_param = rbs_types_function_param_new(&state->allocator, loc, type, NULL);
    return true;
  } else {
    range name_range = state->next_token.range;

    parser_advance(state);

    range param_range = {
      .start = type_range.start,
      .end = name_range.end,
    };

    if (!is_keyword_token(state->current_token.type)) {
      set_error(state, state->current_token, true, "unexpected token for function parameter name");
      return false;
    }

    rbs_string_t unquoted_str = rbs_unquote_string(&state->allocator, rbs_parser_peek_current_token(state));
    rbs_location_t *symbolLoc = rbs_location_current_token(state);
    rbs_constant_id_t constant_id = rbs_constant_pool_insert_string(&state->constant_pool, unquoted_str);
    rbs_ast_symbol_t *name = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, constant_id);

    rbs_location_t *loc = rbs_location_new(&state->allocator, param_range);
    rbs_loc_alloc_children(&state->allocator, loc, 1);
    rbs_loc_add_optional_child(loc, INTERN("name"), name_range);

    *function_param = rbs_types_function_param_new(&state->allocator, loc, type, name);
    return true;
  }
}

static rbs_constant_id_t intern_token_start_end(parserstate *state, token start_token, token end_token) {
  return rbs_constant_pool_insert_shared_with_encoding(
    &state->constant_pool,
    (const uint8_t *) rbs_peek_token(state->lexstate, start_token),
    end_token.range.end.byte_pos - start_token.range.start.byte_pos,
    state->lexstate->encoding
  );
}

/*
  keyword_key ::= {} <keyword> `:`
                | {} keyword <`?`> `:`
*/
NODISCARD
static bool parse_keyword_key(parserstate *state, rbs_ast_symbol_t **key) {
  parser_advance(state);

  rbs_location_t *symbolLoc = rbs_location_current_token(state);

  if (state->next_token.type == pQUESTION) {
    *key = rbs_ast_symbol_new(
      &state->allocator,
      symbolLoc,
      &state->constant_pool,
      intern_token_start_end(state, state->current_token, state->next_token)
    );
    parser_advance(state);
  } else {
    *key = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, INTERN_TOKEN(state, state->current_token));
  }

  return true;
}

/*
  keyword ::= {} keyword `:` <function_param>
*/
NODISCARD
static bool parse_keyword(parserstate *state, rbs_hash_t *keywords, rbs_hash_t *memo) {
  rbs_ast_symbol_t *key = NULL;
  CHECK_PARSE(parse_keyword_key(state, &key));

  if (rbs_hash_find(memo, (rbs_node_t *) key)) {
    set_error(state, state->current_token, true, "duplicated keyword argument");
    return false;
  } else {
    rbs_location_t *loc = rbs_location_current_token(state);
    rbs_hash_set(memo, (rbs_node_t *) key, (rbs_node_t *) rbs_ast_bool_new(&state->allocator, loc, true));
  }

  ADVANCE_ASSERT(state, pCOLON);
  rbs_types_function_param_t *param = NULL;
  CHECK_PARSE(parse_function_param(state, &param));

  rbs_hash_set(keywords, (rbs_node_t *) key, (rbs_node_t *) param);

  return true;
}

/*
Returns true if keyword is given.

  is_keyword === {} KEYWORD `:`
*/
static bool is_keyword(parserstate *state) {
  if (is_keyword_token(state->next_token.type)) {
    if (state->next_token2.type == pCOLON && state->next_token.range.end.byte_pos == state->next_token2.range.start.byte_pos) {
      return true;
    }

    if (state->next_token2.type == pQUESTION
        && state->next_token3.type == pCOLON
        && state->next_token.range.end.byte_pos == state->next_token2.range.start.byte_pos
        && state->next_token2.range.end.byte_pos == state->next_token3.range.start.byte_pos) {
      return true;
    }
  }

  return false;
}

/**
 * Advance token if _next_ token is `type`.
 * Ensures one token advance and `state->current_token.type == type`, or current token not changed.
 *
 * @returns true if token advances, false otherwise.
 **/
static bool parser_advance_if(parserstate *state, enum RBSTokenType type) {
  if (state->next_token.type == type) {
    parser_advance(state);
    return true;
  } else {
    return false;
  }
}

/*
  params ::= {} `)`
           | {} `?` `)`               -- Untyped function params (assign params.required = nil)
           | <required_params> `)`
           | <required_params> `,` `)`

  required_params ::= {} function_param `,` <required_params>
                    | {} <function_param>
                    | {} <optional_params>

  optional_params ::= {} `?` function_param `,` <optional_params>
                    | {} `?` <function_param>
                    | {} <rest_params>

  rest_params ::= {} `*` function_param `,` <trailing_params>
                | {} `*` <function_param>
                | {} <trailing_params>

  trailing_params ::= {} function_param `,` <trailing_params>
                    | {} <function_param>
                    | {} <keywords>

  keywords ::= {} required_keyword `,` <keywords>
             | {} `?` optional_keyword `,` <keywords>
             | {} `**` function_param `,` <keywords>
             | {} <required_keyword>
             | {} `?` <optional_keyword>
             | {} `**` <function_param>
*/
NODISCARD
static bool parse_params(parserstate *state, method_params *params) {
  if (state->next_token.type == pQUESTION && state->next_token2.type == pRPAREN) {
    params->required_positionals = NULL;
    parser_advance(state);
    return true;
  }
  if (state->next_token.type == pRPAREN) {
    return true;
  }

  rbs_hash_t *memo = rbs_hash_new(&state->allocator);

  while (true) {
    switch (state->next_token.type) {
      case pQUESTION:
        goto PARSE_OPTIONAL_PARAMS;
      case pSTAR:
        goto PARSE_REST_PARAM;
      case pSTAR2:
        goto PARSE_KEYWORDS;
      case pRPAREN:
        goto EOP;

      default:
        if (is_keyword(state)) {
          goto PARSE_KEYWORDS;
        }

        rbs_types_function_param_t *param = NULL;
        CHECK_PARSE(parse_function_param(state, &param));
        rbs_node_list_append(params->required_positionals, (rbs_node_t *)param);

        break;
    }

    if (!parser_advance_if(state, pCOMMA)) {
      goto EOP;
    }
  }

PARSE_OPTIONAL_PARAMS:
  while (true) {
    switch (state->next_token.type) {
      case pQUESTION:
        parser_advance(state);

        if (is_keyword(state)) {
          CHECK_PARSE(parse_keyword(state, params->optional_keywords, memo));
          parser_advance_if(state, pCOMMA);
          goto PARSE_KEYWORDS;
        }

        rbs_types_function_param_t *param = NULL;
        CHECK_PARSE(parse_function_param(state, &param));
        rbs_node_list_append(params->optional_positionals, (rbs_node_t *)param);

        break;
      default:
        goto PARSE_REST_PARAM;
    }

    if (!parser_advance_if(state, pCOMMA)) {
      goto EOP;
    }
  }

PARSE_REST_PARAM:
  if (state->next_token.type == pSTAR) {
    parser_advance(state);
    rbs_types_function_param_t *param = NULL;
    CHECK_PARSE(parse_function_param(state, &param));
    params->rest_positionals = (rbs_node_t *) param;

    if (!parser_advance_if(state, pCOMMA)) {
      goto EOP;
    }
  }
  goto PARSE_TRAILING_PARAMS;

PARSE_TRAILING_PARAMS:
  while (true) {
    switch (state->next_token.type) {
      case pQUESTION:
        goto PARSE_KEYWORDS;
      case pSTAR:
        goto EOP;
      case pSTAR2:
        goto PARSE_KEYWORDS;
      case pRPAREN:
        goto EOP;

      default:
        if (is_keyword(state)) {
          goto PARSE_KEYWORDS;
        }

        rbs_types_function_param_t *param = NULL;
        CHECK_PARSE(parse_function_param(state, &param));
        rbs_node_list_append(params->trailing_positionals, (rbs_node_t *)param);

        break;
    }

    if (!parser_advance_if(state, pCOMMA)) {
      goto EOP;
    }
  }

PARSE_KEYWORDS:
  while (true) {
    switch (state->next_token.type) {
    case pQUESTION:
      parser_advance(state);
      if (is_keyword(state)) {
        CHECK_PARSE(parse_keyword(state, params->optional_keywords, memo));
      } else {
        set_error(state, state->next_token, true, "optional keyword argument type is expected");
        return false;
      }
      break;

    case pSTAR2:
      parser_advance(state);
      rbs_types_function_param_t *param = NULL;
      CHECK_PARSE(parse_function_param(state, &param));
      params->rest_keywords = (rbs_node_t *) param;
      break;

    case tUIDENT:
    case tLIDENT:
    case tQIDENT:
    case tULIDENT:
    case tULLIDENT:
    case tBANGIDENT:
    KEYWORD_CASES
      if (is_keyword(state)) {
        CHECK_PARSE(parse_keyword(state, params->required_keywords, memo));
      } else {
        set_error(state, state->next_token, true, "required keyword argument type is expected");
        return false;
      }
      break;

    default:
      goto EOP;
    }

    if (!parser_advance_if(state, pCOMMA)) {
      goto EOP;
    }
  }

EOP:
  if (state->next_token.type != pRPAREN) {
    set_error(state, state->next_token, true, "unexpected token for method type parameters");
    return false;
  }

  return true;
}

/*
  optional ::= {} <simple_type>
             | {} simple_type <`?`>
*/
NODISCARD
static bool parse_optional(parserstate *state, rbs_node_t **optional) {
  range rg;
  rg.start = state->next_token.range.start;

  rbs_node_t *type = NULL;
  CHECK_PARSE(parse_simple(state, &type));

  if (state->next_token.type == pQUESTION) {
    parser_advance(state);
    rg.end = state->current_token.range.end;
    rbs_location_t *location = rbs_location_new(&state->allocator, rg);
    *optional = (rbs_node_t *) rbs_types_optional_new(&state->allocator, location, type);
  } else {
    *optional = type;
  }

  return true;
}

static void initialize_method_params(method_params *params, rbs_allocator_t *allocator) {
  *params = (method_params) {
    .required_positionals = rbs_node_list_new(allocator),
    .optional_positionals = rbs_node_list_new(allocator),
    .rest_positionals = NULL,
    .trailing_positionals = rbs_node_list_new(allocator),
    .required_keywords = rbs_hash_new(allocator),
    .optional_keywords = rbs_hash_new(allocator),
    .rest_keywords = NULL,
  };
}

/*
  self_type_binding ::= {} <>
                      | {} `[` `self` `:` type <`]`>
*/
NODISCARD
static bool parse_self_type_binding(parserstate *state, rbs_node_t **self_type) {
  if (state->next_token.type == pLBRACKET) {
    parser_advance(state);
    ADVANCE_ASSERT(state, kSELF);
    ADVANCE_ASSERT(state, pCOLON);
    rbs_node_t *type;
    CHECK_PARSE(parse_type(state, &type));
    ADVANCE_ASSERT(state, pRBRACKET);
    *self_type = type;
  }

  return true;
}

typedef struct {
  rbs_node_t *function;
  rbs_types_block_t *block;
  rbs_node_t *function_self_type;
} parse_function_result;

/*
  function ::= {} `(` params `)` self_type_binding? `{` `(` params `)` self_type_binding? `->` optional `}` `->` <optional>
             | {} `(` params `)` self_type_binding? `->` <optional>
             | {} self_type_binding? `{` `(` params `)` self_type_binding? `->` optional `}` `->` <optional>
             | {} self_type_binding? `{` self_type_binding `->` optional `}` `->` <optional>
             | {} self_type_binding? `->` <optional>
*/
NODISCARD
static bool parse_function(parserstate *state, bool accept_type_binding, parse_function_result **result) {
  rbs_node_t *function = NULL;
  rbs_types_block_t *block = NULL;
  rbs_node_t *function_self_type = NULL;
  range function_range;
  function_range.start = state->current_token.range.start;

  method_params params;
  initialize_method_params(&params, &state->allocator);

  if (state->next_token.type == pLPAREN) {
    parser_advance(state);
    CHECK_PARSE(parse_params(state, &params));
    ADVANCE_ASSERT(state, pRPAREN);
  }

  // Passing NULL to function_self_type means the function itself doesn't accept self type binding. (== method type)
  if (accept_type_binding) {
    CHECK_PARSE(parse_self_type_binding(state, &function_self_type));
  } else {
    if (rbs_is_untyped_params(&params)) {
      if (state->next_token.type != pARROW) {
        set_error(state, state->next_token2, true, "A method type with untyped method parameter cannot have block");
        return false;
      }
    }
  }

  bool required = true;
  if (state->next_token.type == pQUESTION && state->next_token2.type == pLBRACE) {
    // Optional block
    required = false;
    parser_advance(state);
  }
  if (state->next_token.type == pLBRACE) {
    parser_advance(state);

    method_params block_params;
    initialize_method_params(&block_params, &state->allocator);

    if (state->next_token.type == pLPAREN) {
      parser_advance(state);
      CHECK_PARSE(parse_params(state, &block_params));
      ADVANCE_ASSERT(state, pRPAREN);
    }

    rbs_node_t *self_type = NULL;
    CHECK_PARSE(parse_self_type_binding(state, &self_type));

    ADVANCE_ASSERT(state, pARROW);
    rbs_node_t *block_return_type = NULL;
    CHECK_PARSE(parse_optional(state, &block_return_type));

    rbs_node_t *block_function = NULL;
    function_range.end = state->current_token.range.end;
    rbs_location_t *loc = rbs_location_new(&state->allocator, function_range);
    if (rbs_is_untyped_params(&block_params)) {
      block_function = (rbs_node_t *) rbs_types_untypedfunction_new(&state->allocator, loc, block_return_type);
    } else {
      block_function = (rbs_node_t *) rbs_types_function_new(
        &state->allocator,
        loc,
        block_params.required_positionals,
        block_params.optional_positionals,
        block_params.rest_positionals,
        block_params.trailing_positionals,
        block_params.required_keywords,
        block_params.optional_keywords,
        block_params.rest_keywords,
        block_return_type
      );
    }

    block = rbs_types_block_new(&state->allocator, loc, block_function, required, self_type);

    ADVANCE_ASSERT(state, pRBRACE);
  }

  ADVANCE_ASSERT(state, pARROW);
  rbs_node_t *type = NULL;
  CHECK_PARSE(parse_optional(state, &type));

  function_range.end = state->current_token.range.end;
  rbs_location_t *loc = rbs_location_new(&state->allocator, function_range);
  if (rbs_is_untyped_params(&params)) {
    function = (rbs_node_t *) rbs_types_untypedfunction_new(&state->allocator, loc, type);
  } else {
    function = (rbs_node_t *) rbs_types_function_new(
      &state->allocator,
      loc,
      params.required_positionals,
      params.optional_positionals,
      params.rest_positionals,
      params.trailing_positionals,
      params.required_keywords,
      params.optional_keywords,
      params.rest_keywords,
      type
    );
  }

  (*result)->function = function;
  (*result)->block = block;
  (*result)->function_self_type = function_self_type;
  return true;
}

/*
  proc_type ::= {`^`} <function>
*/
NODISCARD
static bool parse_proc_type(parserstate *state, rbs_types_proc_t **proc) {
  position start = state->current_token.range.start;
  parse_function_result *result = rbs_allocator_alloc(&state->allocator, parse_function_result);
  CHECK_PARSE(parse_function(state, true, &result));

  position end = state->current_token.range.end;
  rbs_location_t *loc = rbs_location_new(&state->allocator, (range) { .start = start, .end = end });
  *proc = rbs_types_proc_new(&state->allocator, loc, result->function, result->block, result->function_self_type);
  return true;
}

static void check_key_duplication(parserstate *state, rbs_hash_t *fields, rbs_node_t *key) {
  if (rbs_hash_find(fields, ((rbs_node_t *) key))) {
    set_error(state, state->current_token, true, "duplicated record key");
  }
}

/**
 * ... `{` ... `}` ...
 *        >   >
 * */
/*
  record_attributes ::= {`{`} record_attribute... <record_attribute> `}`

  record_attribute ::= {} keyword_token `:` <type>
                     | {} literal_type `=>` <type>
*/
NODISCARD
static bool parse_record_attributes(parserstate *state, rbs_hash_t **fields) {
  *fields = rbs_hash_new(&state->allocator);

  if (state->next_token.type == pRBRACE) return true;

  while (true) {
    rbs_ast_symbol_t *key = NULL;
    bool required = true;

    if (state->next_token.type == pQUESTION) {
      // { ?foo: type } syntax
      required = false;
      parser_advance(state);
    }

    if (is_keyword(state)) {
      // { foo: type } syntax
      CHECK_PARSE(parse_keyword_key(state, &key));

      check_key_duplication(state, *fields, (rbs_node_t *) key);
      ADVANCE_ASSERT(state, pCOLON);
    } else {
      // { key => type } syntax
      switch (state->next_token.type) {
      case tSYMBOL:
      case tSQSYMBOL:
      case tDQSYMBOL:
      case tSQSTRING:
      case tDQSTRING:
      case tINTEGER:
      case kTRUE:
      case kFALSE: {
        rbs_node_t *type = NULL;
        CHECK_PARSE(parse_simple(state, &type));

        key = (rbs_ast_symbol_t *) ((rbs_types_literal_t *) type)->literal;
        break;
      }
      default:
        set_error(state, state->next_token, true, "unexpected record key token");
        return false;
      }
      check_key_duplication(state, *fields, (rbs_node_t *) key);
      ADVANCE_ASSERT(state, pFATARROW);
    }

    range field_range;
    field_range.start = state->current_token.range.end;

    rbs_node_t *type;
    CHECK_PARSE(parse_type(state, &type));

    field_range.end = state->current_token.range.end;
    rbs_location_t *loc = rbs_location_new(&state->allocator, field_range);
    rbs_hash_set(*fields, (rbs_node_t *) key, (rbs_node_t *) rbs_types_record_fieldtype_new(&state->allocator, loc, type, required));

    if (parser_advance_if(state, pCOMMA)) {
      if (state->next_token.type == pRBRACE) {
        break;
      }
    } else {
      break;
    }
  }
  return true;
}

/*
  symbol ::= {<tSYMBOL>}
*/
NODISCARD
static bool parse_symbol(parserstate *state, rbs_location_t *location, rbs_types_literal_t **symbol) {
  size_t offset_bytes = state->lexstate->encoding->char_width((const uint8_t *) ":", (size_t) 1);
  size_t bytes = rbs_token_bytes(state->current_token) - offset_bytes;

  rbs_ast_symbol_t *literal;

  switch (state->current_token.type)
  {
  case tSYMBOL: {
    rbs_location_t *symbolLoc = rbs_location_current_token(state);

    char *buffer = rbs_peek_token(state->lexstate, state->current_token);
    rbs_constant_id_t constant_id = rbs_constant_pool_insert_shared(
      &state->constant_pool,
      (const uint8_t *) buffer+offset_bytes,
      bytes
    );
    literal = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, constant_id);
    break;
  }
  case tDQSYMBOL:
  case tSQSYMBOL: {
    rbs_location_t *symbolLoc = rbs_location_current_token(state);
    rbs_string_t current_token = rbs_parser_peek_current_token(state);

    rbs_string_t symbol = rbs_string_new(current_token.start + offset_bytes, current_token.end);

    rbs_string_t unquoted_symbol = rbs_unquote_string(&state->allocator, symbol);

    rbs_constant_id_t constant_id = rbs_constant_pool_insert_string(&state->constant_pool, unquoted_symbol);

    literal = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, constant_id);
    break;
  }
  default:
    set_error(state, state->current_token, false, "Unexpected error");
    return false;
  }

  *symbol = rbs_types_literal_new(&state->allocator, location, (rbs_node_t *) literal);
  return true;
}

/*
 instance_type ::= {type_name} <type_args>

 type_args ::= {} <> /empty/
             | {} `[` type_list <`]`>
 */
NODISCARD
static bool parse_instance_type(parserstate *state, bool parse_alias, rbs_node_t **type) {
    TypeNameKind expected_kind = INTERFACE_NAME | CLASS_NAME;
    if (parse_alias) {
      expected_kind |= ALIAS_NAME;
    }

    range name_range;
    rbs_typename_t *typename = NULL;
    CHECK_PARSE(parse_type_name(state, expected_kind, &name_range, &typename));

    rbs_node_list_t *types = rbs_node_list_new(&state->allocator);

    TypeNameKind kind;
    if (state->current_token.type == tUIDENT) {
      kind = CLASS_NAME;
    } else if (state->current_token.type == tULIDENT) {
      kind = INTERFACE_NAME;
    } else if (state->current_token.type == tLIDENT) {
      kind = ALIAS_NAME;
    } else {
      set_error(state, state->current_token, false, "Unexpected error");
      return false;
    }

    range args_range;
    if (state->next_token.type == pLBRACKET) {
      parser_advance(state);
      args_range.start = state->current_token.range.start;
      CHECK_PARSE(parse_type_list(state, pRBRACKET, types));
      ADVANCE_ASSERT(state, pRBRACKET);
      args_range.end = state->current_token.range.end;
    } else {
      args_range = NULL_RANGE;
    }

    range type_range = {
      .start = name_range.start,
      .end = rbs_nonnull_pos_or(args_range.end, name_range.end),
    };

    rbs_location_t *loc = rbs_location_new(&state->allocator, type_range);
    rbs_loc_alloc_children(&state->allocator, loc, 2);
    rbs_loc_add_required_child(loc, INTERN("name"), name_range);
    rbs_loc_add_optional_child(loc, INTERN("args"), args_range);

    if (kind == CLASS_NAME) {
      *type = (rbs_node_t *) rbs_types_classinstance_new(&state->allocator, loc, typename, types);
    } else if (kind == INTERFACE_NAME) {
      *type = (rbs_node_t *) rbs_types_interface_new(&state->allocator, loc, typename, types);
    } else if (kind == ALIAS_NAME) {
      *type = (rbs_node_t *) rbs_types_alias_new(&state->allocator, loc, typename, types);
    }

    return true;
}

/*
  singleton_type ::= {`singleton`} `(` type_name <`)`>
*/
NODISCARD
static bool parse_singleton_type(parserstate *state, rbs_types_classsingleton_t **singleton) {
  ASSERT_TOKEN(state, kSINGLETON);

  range type_range;
  type_range.start = state->current_token.range.start;
  ADVANCE_ASSERT(state, pLPAREN);
  parser_advance(state);

  range name_range;
  rbs_typename_t *typename = NULL;
  CHECK_PARSE(parse_type_name(state, CLASS_NAME, &name_range, &typename));

  ADVANCE_ASSERT(state, pRPAREN);
  type_range.end = state->current_token.range.end;

  rbs_location_t *loc = rbs_location_new(&state->allocator, type_range);
  rbs_loc_alloc_children(&state->allocator, loc, 1);
  rbs_loc_add_required_child(loc, INTERN("name"), name_range);

  *singleton = rbs_types_classsingleton_new(&state->allocator, loc, typename);
  return true;
}

/*
  simple ::= {} `(` type <`)`>
           | {} <base type>
           | {} <type_name>
           | {} class_instance `[` type_list <`]`>
           | {} `singleton` `(` type_name <`)`>
           | {} `[` type_list <`]`>
           | {} `{` record_attributes <`}`>
           | {} `^` <function>
*/
NODISCARD
static bool parse_simple(parserstate *state, rbs_node_t **type) {
  parser_advance(state);

  switch (state->current_token.type) {
  case pLPAREN: {
    rbs_node_t *lparen_type;
    CHECK_PARSE(parse_type(state, &lparen_type));
    ADVANCE_ASSERT(state, pRPAREN);
    *type = lparen_type;
    return true;
  }
  case kBOOL: {
    rbs_location_t *loc = rbs_location_current_token(state);
    *type = (rbs_node_t *) rbs_types_bases_bool_new(&state->allocator, loc);
    return true;
  }
  case kBOT: {
    rbs_location_t *loc = rbs_location_current_token(state);
    *type = (rbs_node_t *) rbs_types_bases_bottom_new(&state->allocator, loc);
    return true;
  }
  case kCLASS: {
    rbs_location_t *loc = rbs_location_current_token(state);
    *type = (rbs_node_t *) rbs_types_bases_class_new(&state->allocator, loc);
    return true;
  }
  case kINSTANCE: {
    rbs_location_t *loc = rbs_location_current_token(state);
    *type = (rbs_node_t *) rbs_types_bases_instance_new(&state->allocator, loc);
    return true;
  }
  case kNIL: {
    rbs_location_t *loc = rbs_location_current_token(state);
    *type = (rbs_node_t *) rbs_types_bases_nil_new(&state->allocator, loc);
    return true;
  }
  case kSELF: {
    rbs_location_t *loc = rbs_location_current_token(state);
    *type = (rbs_node_t *) rbs_types_bases_self_new(&state->allocator, loc);
    return true;
  }
  case kTOP: {
    rbs_location_t *loc = rbs_location_current_token(state);
    *type = (rbs_node_t *) rbs_types_bases_top_new(&state->allocator, loc);
    return true;
  }
  case kVOID: {
    rbs_location_t *loc = rbs_location_current_token(state);
    *type = (rbs_node_t *) rbs_types_bases_void_new(&state->allocator, loc);
    return true;
  }
  case kUNTYPED: {
    rbs_location_t *loc = rbs_location_current_token(state);
    *type = (rbs_node_t *) rbs_types_bases_any_new(&state->allocator, loc, false);
    return true;
  }
  case k__TODO__: {
    rbs_location_t *loc = rbs_location_current_token(state);
    *type = (rbs_node_t *) rbs_types_bases_any_new(&state->allocator, loc, true);
    return true;
  }
  case tINTEGER: {
    rbs_location_t *loc = rbs_location_current_token(state);

    rbs_string_t string = rbs_parser_peek_current_token(state);
    rbs_string_t stripped_string = rbs_string_strip_whitespace( &string);

    rbs_node_t *literal = (rbs_node_t *) rbs_ast_integer_new(&state->allocator, loc, stripped_string);
    *type = (rbs_node_t *) rbs_types_literal_new(&state->allocator, loc, literal);
    return true;
  }
  case kTRUE: {
    rbs_location_t *loc = rbs_location_current_token(state);
    *type = (rbs_node_t *) rbs_types_literal_new(&state->allocator, loc, (rbs_node_t *) rbs_ast_bool_new(&state->allocator, loc, true));
    return true;
  }
  case kFALSE: {
    rbs_location_t *loc = rbs_location_current_token(state);
    *type = (rbs_node_t *) rbs_types_literal_new(&state->allocator, loc, (rbs_node_t *) rbs_ast_bool_new(&state->allocator, loc, false));
    return true;
  }
  case tSQSTRING:
  case tDQSTRING: {
    rbs_location_t *loc = rbs_location_current_token(state);

    rbs_string_t unquoted_str = rbs_unquote_string(&state->allocator, rbs_parser_peek_current_token(state));
    rbs_node_t *literal = (rbs_node_t *) rbs_ast_string_new(&state->allocator, loc, unquoted_str);
    *type = (rbs_node_t *) rbs_types_literal_new(&state->allocator, loc, literal);
    return true;
  }
  case tSYMBOL:
  case tSQSYMBOL:
  case tDQSYMBOL: {
    rbs_location_t *loc = rbs_location_current_token(state);
    rbs_types_literal_t *literal = NULL;
    CHECK_PARSE(parse_symbol(state, loc, &literal));
    *type = (rbs_node_t *) literal;
    return true;
  }
  case tUIDENT: {
    const char *name_str = rbs_peek_token(state->lexstate, state->current_token);
    size_t name_len = rbs_token_bytes(state->current_token);

    rbs_constant_id_t name = rbs_constant_pool_find(&state->constant_pool, (const uint8_t *) name_str, name_len);

    if (parser_typevar_member(state, name)) {
      rbs_location_t *loc = rbs_location_current_token(state);
      rbs_ast_symbol_t *symbol = rbs_ast_symbol_new(&state->allocator, loc, &state->constant_pool, name);
      *type = (rbs_node_t *) rbs_types_variable_new(&state->allocator, loc, symbol);
      return true;
    }

    RBS_FALLTHROUGH // for type name
  }
  case tULIDENT:
  case tLIDENT:
  case pCOLON2: {
    rbs_node_t *instance_type = NULL;
    CHECK_PARSE(parse_instance_type(state, true, &instance_type));
    *type = instance_type;
    return true;
  }
  case kSINGLETON: {
    rbs_types_classsingleton_t *singleton = NULL;
    CHECK_PARSE(parse_singleton_type(state, &singleton));
    *type = (rbs_node_t *) singleton;
    return true;
  }
  case pLBRACKET: {
    range rg;
    rg.start = state->current_token.range.start;
    rbs_node_list_t *types = rbs_node_list_new(&state->allocator);
    if (state->next_token.type != pRBRACKET) {
      CHECK_PARSE(parse_type_list(state, pRBRACKET, types));
    }
    ADVANCE_ASSERT(state, pRBRACKET);
    rg.end = state->current_token.range.end;

    rbs_location_t *loc = rbs_location_new(&state->allocator, rg);
    *type = (rbs_node_t *) rbs_types_tuple_new(&state->allocator, loc, types);
    return true;
  }
  case pAREF_OPR: {
    rbs_location_t *loc = rbs_location_current_token(state);
    rbs_node_list_t *types = rbs_node_list_new(&state->allocator);
    *type = (rbs_node_t *) rbs_types_tuple_new(&state->allocator, loc, types);
    return true;
  }
  case pLBRACE: {
    position start = state->current_token.range.start;
    rbs_hash_t *fields = NULL;
    CHECK_PARSE(parse_record_attributes(state, &fields));
    ADVANCE_ASSERT(state, pRBRACE);
    position end = state->current_token.range.end;
    rbs_location_t *loc = rbs_location_new(&state->allocator, (range) { .start = start, .end = end });
    *type = (rbs_node_t *) rbs_types_record_new(&state->allocator, loc, fields);
    return true;
  }
  case pHAT: {
    rbs_types_proc_t *value = NULL;
    CHECK_PARSE(parse_proc_type(state, &value));
    *type = (rbs_node_t *) value;
    return true;
  }
  default:
    set_error(state, state->current_token, true, "unexpected token for simple type");
    return false;
  }
}

/*
  intersection ::= {} optional `&` ... '&' <optional>
                 | {} <optional>
*/
NODISCARD
static bool parse_intersection(parserstate *state, rbs_node_t **type) {
  range rg;
  rg.start = state->next_token.range.start;

  rbs_node_t *optional = NULL;
  CHECK_PARSE(parse_optional(state, &optional));
  *type = optional;

  rbs_node_list_t *intersection_types = rbs_node_list_new(&state->allocator);

  rbs_node_list_append(intersection_types, optional);
  while (state->next_token.type == pAMP) {
    parser_advance(state);
    rbs_node_t *type = NULL;
    CHECK_PARSE(parse_optional(state, &type));
    rbs_node_list_append(intersection_types, type);
  }

  rg.end = state->current_token.range.end;

  if (intersection_types->length > 1) {
    rbs_location_t *location = rbs_location_new(&state->allocator, rg);
    *type = (rbs_node_t *) rbs_types_intersection_new(&state->allocator, location, intersection_types);
  }

  return true;
}

/*
  union ::= {} intersection '|' ... '|' <intersection>
          | {} <intersection>
*/
bool parse_type(parserstate *state, rbs_node_t **type) {
  range rg;
  rg.start = state->next_token.range.start;
  rbs_node_list_t *union_types = rbs_node_list_new(&state->allocator);

  CHECK_PARSE(parse_intersection(state, type));

  rbs_node_list_append(union_types, *type);

  while (state->next_token.type == pBAR) {
    parser_advance(state);
    rbs_node_t *intersection = NULL;
    CHECK_PARSE(parse_intersection(state, &intersection));
    rbs_node_list_append(union_types, intersection);
  }

  rg.end = state->current_token.range.end;

  if (union_types->length > 1) {
    rbs_location_t *location = rbs_location_new(&state->allocator, rg);
    *type = (rbs_node_t *) rbs_types_union_new(&state->allocator, location, union_types);
  }

  return true;
}

/*
  type_params ::= {} `[` type_param `,` ... <`]`>
                | {<>}

  type_param ::= kUNCHECKED? (kIN|kOUT|) tUIDENT upper_bound? default_type?   (module_type_params == true)

  type_param ::= tUIDENT upper_bound? default_type?                           (module_type_params == false)
*/
NODISCARD
static bool parse_type_params(parserstate *state, range *rg, bool module_type_params, rbs_node_list_t **params) {
  *params = rbs_node_list_new(&state->allocator);

  bool required_param_allowed = true;

  if (state->next_token.type == pLBRACKET) {
    parser_advance(state);

    rg->start = state->current_token.range.start;

    while (true) {
      bool unchecked = false;
      rbs_keyword_t *variance = rbs_keyword_new(&state->allocator, rbs_location_current_token(state), INTERN("invariant"));
      rbs_node_t *upper_bound = NULL;
      rbs_node_t *default_type = NULL;

      range param_range;
      param_range.start = state->next_token.range.start;

      range unchecked_range = NULL_RANGE;
      range variance_range = NULL_RANGE;
      if (module_type_params) {
        if (state->next_token.type == kUNCHECKED) {
          unchecked = true;
          parser_advance(state);
          unchecked_range = state->current_token.range;
        }

        if (state->next_token.type == kIN || state->next_token.type == kOUT) {
          switch (state->next_token.type) {
          case kIN:
            variance = rbs_keyword_new(&state->allocator, rbs_location_current_token(state), INTERN("contravariant"));
            break;
          case kOUT:
            variance = rbs_keyword_new(&state->allocator, rbs_location_current_token(state), INTERN("covariant"));
            break;
          default:
            set_error(state, state->current_token, false, "Unexpected error");
            return false;
          }

          parser_advance(state);
          variance_range = state->current_token.range;
        }
      }

      ADVANCE_ASSERT(state, tUIDENT);
      range name_range = state->current_token.range;

      rbs_string_t string = rbs_parser_peek_current_token(state);
      rbs_location_t *nameSymbolLoc = rbs_location_current_token(state);
      rbs_constant_id_t id = rbs_constant_pool_insert_string(&state->constant_pool, string);
      rbs_ast_symbol_t *name = rbs_ast_symbol_new(&state->allocator, nameSymbolLoc, &state->constant_pool, id);

      CHECK_PARSE(parser_insert_typevar(state, id));

      range upper_bound_range = NULL_RANGE;
      if (state->next_token.type == pLT) {
        parser_advance(state);
        upper_bound_range.start = state->current_token.range.start;
        CHECK_PARSE(parse_type(state, &upper_bound));
        upper_bound_range.end = state->current_token.range.end;
      }

      range default_type_range = NULL_RANGE;
      if (module_type_params) {
        if (state->next_token.type == pEQ) {
          parser_advance(state);

          default_type_range.start = state->current_token.range.start;
          CHECK_PARSE(parse_type(state, &default_type));
          default_type_range.end = state->current_token.range.end;

          required_param_allowed = false;
        } else {
          if (!required_param_allowed) {
            set_error(state, state->current_token, true, "required type parameter is not allowed after optional type parameter");
            return false;
          }
        }
      }

      param_range.end = state->current_token.range.end;

      rbs_location_t *loc = rbs_location_new(&state->allocator, param_range);
      rbs_loc_alloc_children(&state->allocator, loc, 5);
      rbs_loc_add_required_child(loc, INTERN("name"), name_range);
      rbs_loc_add_optional_child(loc, INTERN("variance"), variance_range);
      rbs_loc_add_optional_child(loc, INTERN("unchecked"), unchecked_range);
      rbs_loc_add_optional_child(loc, INTERN("upper_bound"), upper_bound_range);
      rbs_loc_add_optional_child(loc, INTERN("default"), default_type_range);

      rbs_ast_typeparam_t *param = rbs_ast_typeparam_new(&state->allocator, loc, name, variance, upper_bound, default_type, unchecked);

      rbs_node_list_append(*params, (rbs_node_t *) param);

      if (state->next_token.type == pCOMMA) {
        parser_advance(state);
      }

      if (state->next_token.type == pRBRACKET) {
        break;
      }
    }

    ADVANCE_ASSERT(state, pRBRACKET);
    rg->end = state->current_token.range.end;
  } else {
    *rg = NULL_RANGE;
  }

  return true;
}

/*
  method_type ::= {} type_params <function>
  */
// TODO: Should this be NODISCARD?
bool parse_method_type(parserstate *state, rbs_methodtype_t **method_type) {
  parser_push_typevar_table(state, false);

  range rg;
  rg.start = state->next_token.range.start;

  range params_range = NULL_RANGE;
  rbs_node_list_t *type_params;
  CHECK_PARSE(parse_type_params(state, &params_range, false, &type_params));

  range type_range;
  type_range.start = state->next_token.range.start;

  parse_function_result *result = rbs_allocator_alloc(&state->allocator, parse_function_result);
  CHECK_PARSE(parse_function(state, false, &result));

  rg.end = state->current_token.range.end;
  type_range.end = rg.end;

  CHECK_PARSE(parser_pop_typevar_table(state));

  rbs_location_t *loc = rbs_location_new(&state->allocator, rg);
  rbs_loc_alloc_children(&state->allocator, loc, 2);
  rbs_loc_add_required_child(loc, INTERN("type"), type_range);
  rbs_loc_add_optional_child(loc, INTERN("type_params"), params_range);

  *method_type = rbs_methodtype_new(&state->allocator, loc, type_params, result->function, result->block);
  return true;
}

/*
  global_decl ::= {tGIDENT} `:` <type>
*/
NODISCARD
static bool parse_global_decl(parserstate *state, rbs_node_list_t *annotations, rbs_ast_declarations_global_t **global) {
  range decl_range;
  decl_range.start = state->current_token.range.start;

  rbs_ast_comment_t *comment = get_comment(state, decl_range.start.line);

  range name_range = state->current_token.range;
  rbs_location_t *symbolLoc = rbs_location_new(&state->allocator, name_range);

  rbs_ast_symbol_t *typename = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, INTERN_TOKEN(state, state->current_token));

  ADVANCE_ASSERT(state, pCOLON);
  range colon_range = state->current_token.range;

  rbs_node_t *type;
  CHECK_PARSE(parse_type(state, &type));
  decl_range.end = state->current_token.range.end;

  rbs_location_t *loc = rbs_location_new(&state->allocator, decl_range);
  rbs_loc_alloc_children(&state->allocator, loc, 2);
  rbs_loc_add_required_child(loc, INTERN("name"), name_range);
  rbs_loc_add_required_child(loc, INTERN("colon"), colon_range);

  *global = rbs_ast_declarations_global_new(&state->allocator, loc, typename, type, comment, annotations);
  return true;
}

/*
  const_decl ::= {const_name} `:` <type>
*/
NODISCARD
static bool parse_const_decl(parserstate *state, rbs_node_list_t *annotations, rbs_ast_declarations_constant_t **constant) {
  range decl_range;

  decl_range.start = state->current_token.range.start;
  rbs_ast_comment_t *comment = get_comment(state, decl_range.start.line);

  range name_range;
  rbs_typename_t *typename = NULL;
  CHECK_PARSE(parse_type_name(state, CLASS_NAME, &name_range, &typename));

  ADVANCE_ASSERT(state, pCOLON);
  range colon_range = state->current_token.range;

  rbs_node_t *type;
  CHECK_PARSE(parse_type(state, &type));

  decl_range.end = state->current_token.range.end;

  rbs_location_t *loc = rbs_location_new(&state->allocator, decl_range);
  rbs_loc_alloc_children(&state->allocator, loc, 2);
  rbs_loc_add_required_child(loc, INTERN("name"), name_range);
  rbs_loc_add_required_child(loc, INTERN("colon"), colon_range);

  *constant = rbs_ast_declarations_constant_new(&state->allocator, loc, typename, type, comment, annotations);
  return true;
}

/*
  type_decl ::= {kTYPE} alias_name `=` <type>
*/
NODISCARD
static bool parse_type_decl(parserstate *state, position comment_pos, rbs_node_list_t *annotations, rbs_ast_declarations_typealias_t **typealias) {
  parser_push_typevar_table(state, true);

  range decl_range;
  decl_range.start = state->current_token.range.start;
  comment_pos = rbs_nonnull_pos_or(comment_pos, decl_range.start);

  range keyword_range = state->current_token.range;

  parser_advance(state);

  range name_range;
  rbs_typename_t *typename = NULL;
  CHECK_PARSE(parse_type_name(state, ALIAS_NAME, &name_range, &typename));

  range params_range;
  rbs_node_list_t *type_params;
  CHECK_PARSE(parse_type_params(state, &params_range, true, &type_params));

  ADVANCE_ASSERT(state, pEQ);
  range eq_range = state->current_token.range;

  rbs_node_t *type;
  CHECK_PARSE(parse_type(state, &type));

  decl_range.end = state->current_token.range.end;

  rbs_location_t *loc = rbs_location_new(&state->allocator, decl_range);
  rbs_loc_alloc_children(&state->allocator, loc, 4);
  rbs_loc_add_required_child(loc, INTERN("keyword"), keyword_range);
  rbs_loc_add_required_child(loc, INTERN("name"), name_range);
  rbs_loc_add_optional_child(loc, INTERN("type_params"), params_range);
  rbs_loc_add_required_child(loc, INTERN("eq"), eq_range);

  CHECK_PARSE(parser_pop_typevar_table(state));

  rbs_ast_comment_t *comment = get_comment(state, comment_pos.line);

  *typealias = rbs_ast_declarations_typealias_new(&state->allocator, loc, typename, type_params, type, annotations, comment);
  return true;
}

/*
  annotation ::= {<tANNOTATION>}
*/
NODISCARD
static bool parse_annotation(parserstate *state, rbs_ast_annotation_t **annotation) {
  range rg = state->current_token.range;

  size_t offset_bytes =
    state->lexstate->encoding->char_width((const uint8_t *) "%", (size_t) 1) +
    state->lexstate->encoding->char_width((const uint8_t *) "a", (size_t) 1);

  rbs_string_t str = rbs_string_new(
    state->lexstate->string.start + rg.start.byte_pos + offset_bytes,
    state->lexstate->string.end
  );
  unsigned int open_char = rbs_utf8_to_codepoint(str);

  unsigned int close_char;

  switch (open_char) {
  case '{':
    close_char = '}';
    break;
  case '(':
    close_char = ')';
    break;
  case '[':
    close_char = ']';
    break;
  case '<':
    close_char = '>';
    break;
  case '|':
    close_char = '|';
    break;
  default:
    set_error(state, state->current_token, false, "Unexpected error");
    return false;
  }

  size_t open_bytes = state->lexstate->encoding->char_width((const uint8_t *) &open_char, (size_t) 1);
  size_t close_bytes = state->lexstate->encoding->char_width((const uint8_t *) &close_char, (size_t) 1);

  rbs_string_t current_token = rbs_parser_peek_current_token(state);
  size_t total_offset = offset_bytes + open_bytes;

  rbs_string_t annotation_str = rbs_string_new(
    current_token.start + total_offset,
    current_token.end - close_bytes
  );

  rbs_string_t stripped_annotation_str = rbs_string_strip_whitespace(&annotation_str);

  *annotation = rbs_ast_annotation_new(&state->allocator, rbs_location_new(&state->allocator, rg), stripped_annotation_str);
  return true;
}

/*
  annotations ::= {} annotation ... <annotation>
                | {<>}
*/
NODISCARD
static bool parse_annotations(parserstate *state, rbs_node_list_t *annotations, position *annot_pos) {
  *annot_pos = NullPosition;

  while (true) {
    if (state->next_token.type == tANNOTATION) {
      parser_advance(state);

      if (rbs_null_position_p((*annot_pos))) {
        *annot_pos = state->current_token.range.start;
      }

      rbs_ast_annotation_t *annotation = NULL;
      CHECK_PARSE(parse_annotation(state, &annotation));
      rbs_node_list_append(annotations, (rbs_node_t *) annotation);
    } else {
      break;
    }
  }

  return true;
}

/*
  method_name ::= {} <IDENT | keyword>
                | {} (IDENT | keyword)~<`?`>
*/
NODISCARD
static bool parse_method_name(parserstate *state, range *range, rbs_ast_symbol_t **symbol) {
  parser_advance(state);

  switch (state->current_token.type)
  {
  case tUIDENT:
  case tLIDENT:
  case tULIDENT:
  case tULLIDENT:
  KEYWORD_CASES
    if (state->next_token.type == pQUESTION && state->current_token.range.end.byte_pos == state->next_token.range.start.byte_pos) {
      range->start = state->current_token.range.start;
      range->end = state->next_token.range.end;
      parser_advance(state);

      rbs_constant_id_t constant_id = rbs_constant_pool_insert_shared_with_encoding(
        &state->constant_pool,
        (const uint8_t *) state->lexstate->string.start + range->start.byte_pos,
        range->end.byte_pos - range->start.byte_pos,
        state->lexstate->encoding
      );

      rbs_location_t *symbolLoc = rbs_location_new(&state->allocator, *range);
      *symbol = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, constant_id);
    } else {
      *range = state->current_token.range;
      rbs_location_t *symbolLoc = rbs_location_new(&state->allocator, *range);
      *symbol = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, INTERN_TOKEN(state, state->current_token));
    }
    return true;

  case tBANGIDENT:
  case tEQIDENT: {
    *range = state->current_token.range;
    rbs_location_t *symbolLoc = rbs_location_new(&state->allocator, *range);
    *symbol = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, INTERN_TOKEN(state, state->current_token));
    return true;
  }
  case tQIDENT: {
    rbs_string_t string = rbs_parser_peek_current_token(state);
    rbs_string_t unquoted_str = rbs_unquote_string(&state->allocator, string);
    rbs_constant_id_t constant_id = rbs_constant_pool_insert_string(&state->constant_pool, unquoted_str);
    rbs_location_t *symbolLoc = rbs_location_current_token(state);
    *symbol = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, constant_id);
    return true;
  }

  case pBAR:
  case pHAT:
  case pAMP:
  case pSTAR:
  case pSTAR2:
  case pLT:
  case pAREF_OPR:
  case tOPERATOR: {
    *range = state->current_token.range;
    rbs_location_t *symbolLoc = rbs_location_new(&state->allocator, *range);
    *symbol = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, INTERN_TOKEN(state, state->current_token));
    return true;
  }

  default:
    set_error(state, state->current_token, true, "unexpected token for method name");
    return false;
  }
}

typedef enum {
  INSTANCE_KIND,
  SINGLETON_KIND,
  INSTANCE_SINGLETON_KIND
} InstanceSingletonKind;

/*
  instance_singleton_kind ::= {<>}
                            | {} kSELF <`.`>
                            | {} kSELF~`?` <`.`>

  @param allow_selfq `true` to accept `self?` kind.
*/
static InstanceSingletonKind parse_instance_singleton_kind(parserstate *state, bool allow_selfq, range *rg) {
  InstanceSingletonKind kind = INSTANCE_KIND;

  if (state->next_token.type == kSELF) {
    range self_range = state->next_token.range;

    if (state->next_token2.type == pDOT) {
      parser_advance(state);
      parser_advance(state);
      kind = SINGLETON_KIND;
    } else if (
      state->next_token2.type == pQUESTION
    && state->next_token.range.end.char_pos == state->next_token2.range.start.char_pos
    && state->next_token3.type == pDOT
    && allow_selfq) {
      parser_advance(state);
      parser_advance(state);
      parser_advance(state);
      kind = INSTANCE_SINGLETON_KIND;
    }

    *rg = (range) {
      .start = self_range.start,
      .end = state->current_token.range.end,
    };
  } else {
    *rg = NULL_RANGE;
  }

  return kind;
}

/**
 * def_member ::= {kDEF} method_name `:` <method_types>
 *              | {kPRIVATE} kDEF method_name `:` <method_types>
 *              | {kPUBLIC} kDEF method_name `:` <method_types>
 *
 * method_types ::= {} <method_type>
 *                | {} <`...`>
 *                | {} method_type `|` <method_types>
 *
 * @param instance_only `true` to reject singleton method definition.
 * @param accept_overload `true` to accept overloading (...) definition.
 * */
NODISCARD
static bool parse_member_def(parserstate *state, bool instance_only, bool accept_overload, position comment_pos, rbs_node_list_t *annotations, rbs_ast_members_methoddefinition_t **method_definition) {
  range member_range;
  member_range.start = state->current_token.range.start;
  comment_pos = rbs_nonnull_pos_or(comment_pos, member_range.start);

  rbs_ast_comment_t *comment = get_comment(state, comment_pos.line);

  range visibility_range;
  rbs_keyword_t *visibility;
  switch (state->current_token.type)
  {
  case kPRIVATE: {
    visibility_range = state->current_token.range;
    visibility = rbs_keyword_new(&state->allocator, rbs_location_new(&state->allocator, visibility_range), INTERN("private"));
    member_range.start = visibility_range.start;
    parser_advance(state);
    break;
  }
  case kPUBLIC: {
    visibility_range = state->current_token.range;
    visibility = rbs_keyword_new(&state->allocator, rbs_location_new(&state->allocator, visibility_range), INTERN("public"));
    member_range.start = visibility_range.start;
    parser_advance(state);
    break;
  }
  default:
    visibility_range = NULL_RANGE;
    visibility = NULL;
    break;
  }

  range keyword_range = state->current_token.range;

  range kind_range;
  InstanceSingletonKind kind;
  if (instance_only) {
    kind_range = NULL_RANGE;
    kind = INSTANCE_KIND;
  } else {
    kind = parse_instance_singleton_kind(state, visibility == NULL, &kind_range);
  }

  range name_range;
  rbs_ast_symbol_t *name = NULL;
  CHECK_PARSE(parse_method_name(state, &name_range, &name));

  #define SELF_ID rbs_constant_pool_insert_constant(&state->constant_pool, (const unsigned char *) "self?", strlen("self?"))

  if (state->next_token.type == pDOT && name->constant_id == SELF_ID) {
    set_error(state, state->next_token, true, "`self?` method cannot have visibility");
    return false;
  } else {
    ADVANCE_ASSERT(state, pCOLON);
  }

  parser_push_typevar_table(state, kind != INSTANCE_KIND);

  rbs_node_list_t *overloads = rbs_node_list_new(&state->allocator);
  bool overloading = false;
  range overloading_range = NULL_RANGE;
  bool loop = true;
  while (loop) {
    rbs_node_list_t *annotations = rbs_node_list_new(&state->allocator);
    position overload_annot_pos = NullPosition;

    range overload_range;
    overload_range.start = state->current_token.range.start;

    if (state->next_token.type == tANNOTATION) {
      CHECK_PARSE(parse_annotations(state, annotations, &overload_annot_pos));
    }

    switch (state->next_token.type) {
    case pLPAREN:
    case pARROW:
    case pLBRACE:
    case pLBRACKET:
    case pQUESTION:
      {
        rbs_methodtype_t *method_type = NULL;
        CHECK_PARSE(parse_method_type(state, &method_type));

        overload_range.end = state->current_token.range.end;
        rbs_location_t *loc = rbs_location_new(&state->allocator, overload_range);
        rbs_node_t *overload = (rbs_node_t *) rbs_ast_members_methoddefinition_overload_new(&state->allocator, loc, annotations, (rbs_node_t *) method_type);
        rbs_node_list_append(overloads, overload);
        member_range.end = state->current_token.range.end;
        break;
      }

    case pDOT3:
      if (accept_overload) {
        overloading = true;
        parser_advance(state);
        loop = false;
        overloading_range = state->current_token.range;
        member_range.end = overloading_range.end;
        break;
      } else {
        set_error(state, state->next_token, true, "unexpected overloading method definition");
        return false;
      }

    default:
      set_error(state, state->next_token, true, "unexpected token for method type");
      return false;
    }

    if (state->next_token.type == pBAR) {
      parser_advance(state);
    } else {
      loop = false;
    }
  }

  CHECK_PARSE(parser_pop_typevar_table(state));

  rbs_keyword_t *k;
  switch (kind) {
  case INSTANCE_KIND: {
    k = rbs_keyword_new(&state->allocator, rbs_location_current_token(state), INTERN("instance"));
    break;
  }
  case SINGLETON_KIND: {
    k = rbs_keyword_new(&state->allocator, rbs_location_current_token(state), INTERN("singleton"));
    break;
  }
  case INSTANCE_SINGLETON_KIND: {
    k = rbs_keyword_new(&state->allocator, rbs_location_current_token(state), INTERN("singleton_instance"));
    break;
  }
  default:
    set_error(state, state->current_token, false, "Unexpected error");
    return false;
  }

  rbs_location_t *loc = rbs_location_new(&state->allocator, member_range);
  rbs_loc_alloc_children(&state->allocator, loc, 5);
  rbs_loc_add_required_child(loc, INTERN("keyword"), keyword_range);
  rbs_loc_add_required_child(loc, INTERN("name"), name_range);
  rbs_loc_add_optional_child(loc, INTERN("kind"), kind_range);
  rbs_loc_add_optional_child(loc, INTERN("overloading"), overloading_range);
  rbs_loc_add_optional_child(loc, INTERN("visibility"), visibility_range);

  *method_definition = rbs_ast_members_methoddefinition_new(&state->allocator, loc, name, k, overloads, annotations, comment, overloading, visibility);
  return true;
}

/**
 * class_instance_name ::= {} <class_name>
 *                       | {} class_name `[` type args <`]`>
 *
 * @param kind
 * */
NODISCARD
static bool class_instance_name(parserstate *state, TypeNameKind kind, rbs_node_list_t *args, range *name_range, range *args_range, rbs_typename_t **name) {
  parser_advance(state);

  rbs_typename_t *typename = NULL;
  CHECK_PARSE(parse_type_name(state, kind, name_range, &typename));
  *name = typename;

  if (state->next_token.type == pLBRACKET) {
    parser_advance(state);
    args_range->start = state->current_token.range.start;
    CHECK_PARSE(parse_type_list(state, pRBRACKET, args));
    ADVANCE_ASSERT(state, pRBRACKET);
    args_range->end = state->current_token.range.end;
  } else {
    *args_range = NULL_RANGE;
  }

  return true;
}

/**
 *  mixin_member ::= {kINCLUDE} <class_instance_name>
 *                 | {kPREPEND} <class_instance_name>
 *                 | {kEXTEND} <class_instance_name>
 *
 * @param from_interface `true` when the member is in an interface.
 * */
NODISCARD
static bool parse_mixin_member(parserstate *state, bool from_interface, position comment_pos, rbs_node_list_t *annotations, rbs_node_t **mixin_member) {
  range member_range;
  member_range.start = state->current_token.range.start;
  comment_pos = rbs_nonnull_pos_or(comment_pos, member_range.start);

  enum RBSTokenType type = state->current_token.type;
  range keyword_range = state->current_token.range;

  bool reset_typevar_scope;
  switch (type)
  {
  case kINCLUDE:
    reset_typevar_scope = false;
    break;
  case kEXTEND:
    reset_typevar_scope = true;
    break;
  case kPREPEND:
    reset_typevar_scope = false;
    break;
  default:
    set_error(state, state->current_token, false, "Unexpected error");
    return false;
  }

  if (from_interface) {
    if (state->current_token.type != kINCLUDE) {
      set_error(state, state->current_token, true, "unexpected mixin in interface declaration");
      return false;
    }
  }

  parser_push_typevar_table(state, reset_typevar_scope);

  rbs_node_list_t *args = rbs_node_list_new(&state->allocator);
  range name_range;
  range args_range = NULL_RANGE;
  rbs_typename_t *name = NULL;
  CHECK_PARSE(class_instance_name(
    state,
    from_interface ? INTERFACE_NAME : (INTERFACE_NAME | CLASS_NAME),
    args, &name_range, &args_range, &name
  ));

  CHECK_PARSE(parser_pop_typevar_table(state));

  member_range.end = state->current_token.range.end;

  rbs_location_t *loc = rbs_location_new(&state->allocator, member_range);
  rbs_loc_alloc_children(&state->allocator, loc, 3);
  rbs_loc_add_required_child(loc, INTERN("name"), name_range);
  rbs_loc_add_required_child(loc, INTERN("keyword"), keyword_range);
  rbs_loc_add_optional_child(loc, INTERN("args"), args_range);

  rbs_ast_comment_t *comment = get_comment(state, comment_pos.line);
  switch (type)
  {
  case kINCLUDE:
    *mixin_member = (rbs_node_t *) rbs_ast_members_include_new(&state->allocator, loc, name, args, annotations, comment);
    return true;
  case kEXTEND:
    *mixin_member = (rbs_node_t *) rbs_ast_members_extend_new(&state->allocator, loc, name, args, annotations, comment);
    return true;
  case kPREPEND:
    *mixin_member = (rbs_node_t *) rbs_ast_members_prepend_new(&state->allocator, loc, name, args, annotations, comment);
    return true;
  default:
    set_error(state, state->current_token, false, "Unexpected error");
    return false;
  }
}

/**
 * @code
 * alias_member ::= {kALIAS} method_name <method_name>
 *                | {kALIAS} kSELF `.` method_name kSELF `.` <method_name>
 * @endcode
 *
 * @param[in] instance_only `true` to reject `self.` alias.
 * */
NODISCARD
static bool parse_alias_member(parserstate *state, bool instance_only, position comment_pos, rbs_node_list_t *annotations, rbs_ast_members_alias_t **alias_member) {
  range member_range;
  member_range.start = state->current_token.range.start;
  range keyword_range = state->current_token.range;

  comment_pos = rbs_nonnull_pos_or(comment_pos, member_range.start);
  rbs_ast_comment_t *comment = get_comment(state, comment_pos.line);

  rbs_keyword_t *kind;
  rbs_ast_symbol_t *new_name, *old_name;
  range new_kind_range, old_kind_range, new_name_range, old_name_range;

  if (!instance_only && state->next_token.type == kSELF) {
    kind = rbs_keyword_new(&state->allocator, rbs_location_current_token(state), INTERN("singleton"));

    new_kind_range.start = state->next_token.range.start;
    new_kind_range.end = state->next_token2.range.end;
    ADVANCE_ASSERT(state, kSELF);
    ADVANCE_ASSERT(state, pDOT);
    CHECK_PARSE(parse_method_name(state, &new_name_range, &new_name));

    old_kind_range.start = state->next_token.range.start;
    old_kind_range.end = state->next_token2.range.end;
    ADVANCE_ASSERT(state, kSELF);
    ADVANCE_ASSERT(state, pDOT);
    CHECK_PARSE(parse_method_name(state, &old_name_range, &old_name));
  } else {
    kind = rbs_keyword_new(&state->allocator, rbs_location_current_token(state), INTERN("instance"));
    CHECK_PARSE(parse_method_name(state, &new_name_range, &new_name));
    CHECK_PARSE(parse_method_name(state, &old_name_range, &old_name));
    new_kind_range = NULL_RANGE;
    old_kind_range = NULL_RANGE;
  }

  member_range.end = state->current_token.range.end;
  rbs_location_t *loc = rbs_location_new(&state->allocator, member_range);
  rbs_loc_alloc_children(&state->allocator, loc, 5);
  rbs_loc_add_required_child(loc, INTERN("keyword"), keyword_range);
  rbs_loc_add_required_child(loc, INTERN("new_name"), new_name_range);
  rbs_loc_add_required_child(loc, INTERN("old_name"), old_name_range);
  rbs_loc_add_optional_child(loc, INTERN("new_kind"), new_kind_range);
  rbs_loc_add_optional_child(loc, INTERN("old_kind"), old_kind_range);

  *alias_member = rbs_ast_members_alias_new(&state->allocator, loc, new_name, old_name, kind, annotations, comment);
  return true;
}

/*
  variable_member ::= {tAIDENT} `:` <type>
                    | {kSELF} `.` tAIDENT `:` <type>
                    | {tA2IDENT} `:` <type>
*/
NODISCARD
static bool parse_variable_member(parserstate *state, position comment_pos, rbs_node_list_t *annotations, rbs_node_t **variable_member) {
  if (annotations->length > 0) {
    set_error(state, state->current_token, true, "annotation cannot be given to variable members");
    return false;
  }

  range member_range;
  member_range.start = state->current_token.range.start;
  comment_pos = rbs_nonnull_pos_or(comment_pos, member_range.start);
  rbs_ast_comment_t *comment = get_comment(state, comment_pos.line);

  switch (state->current_token.type)
  {
  case tAIDENT: {
    range name_range = state->current_token.range;
    rbs_location_t *symbolLoc = rbs_location_new(&state->allocator, name_range);
    rbs_ast_symbol_t *name = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, INTERN_TOKEN(state, state->current_token));

    ADVANCE_ASSERT(state, pCOLON);
    range colon_range = state->current_token.range;

    rbs_node_t *type;
    CHECK_PARSE(parse_type(state, &type));
    member_range.end = state->current_token.range.end;

    rbs_location_t *loc = rbs_location_new(&state->allocator, member_range);
    rbs_loc_alloc_children(&state->allocator, loc, 3);
    rbs_loc_add_required_child(loc, INTERN("name"), name_range);
    rbs_loc_add_required_child(loc, INTERN("colon"), colon_range);
    rbs_loc_add_optional_child(loc, INTERN("kind"), NULL_RANGE);

    *variable_member = (rbs_node_t *)rbs_ast_members_instancevariable_new(&state->allocator, loc, name, type, comment);
    return true;
  }
  case tA2IDENT: {
    range name_range = state->current_token.range;
    rbs_location_t *symbolLoc = rbs_location_new(&state->allocator, name_range);
    rbs_ast_symbol_t *name = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, INTERN_TOKEN(state, state->current_token));

    ADVANCE_ASSERT(state, pCOLON);
    range colon_range = state->current_token.range;

    parser_push_typevar_table(state, true);

    rbs_node_t *type;
    CHECK_PARSE(parse_type(state, &type));

    CHECK_PARSE(parser_pop_typevar_table(state));

    member_range.end = state->current_token.range.end;

    rbs_location_t *loc = rbs_location_new(&state->allocator, member_range);
    rbs_loc_alloc_children(&state->allocator, loc, 3);
    rbs_loc_add_required_child(loc, INTERN("name"), name_range);
    rbs_loc_add_required_child(loc, INTERN("colon"), colon_range);
    rbs_loc_add_optional_child(loc, INTERN("kind"), NULL_RANGE);

    *variable_member = (rbs_node_t *) rbs_ast_members_classvariable_new(&state->allocator, loc, name, type, comment);
    return true;
  }
  case kSELF: {
    range kind_range = {
      .start = state->current_token.range.start,
      .end = state->next_token.range.end
    };

    ADVANCE_ASSERT(state, pDOT);
    ADVANCE_ASSERT(state, tAIDENT);

    range name_range = state->current_token.range;
    rbs_location_t *symbolLoc = rbs_location_new(&state->allocator, name_range);
    rbs_ast_symbol_t *name = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, INTERN_TOKEN(state, state->current_token));

    ADVANCE_ASSERT(state, pCOLON);
    range colon_range = state->current_token.range;

    parser_push_typevar_table(state, true);

    rbs_node_t *type;
    CHECK_PARSE(parse_type(state, &type));

    CHECK_PARSE(parser_pop_typevar_table(state));

    member_range.end = state->current_token.range.end;

    rbs_location_t *loc = rbs_location_new(&state->allocator, member_range);
    rbs_loc_alloc_children(&state->allocator, loc, 3);
    rbs_loc_add_required_child(loc, INTERN("name"), name_range);
    rbs_loc_add_required_child(loc, INTERN("colon"), colon_range);
    rbs_loc_add_optional_child(loc, INTERN("kind"), kind_range);

    *variable_member = (rbs_node_t *)rbs_ast_members_classinstancevariable_new(&state->allocator, loc, name, type, comment);
    return true;
  }
  default:
    set_error(state, state->current_token, false, "Unexpected error");
    return false;
  }
}

/*
  visibility_member ::= {<`public`>}
                      | {<`private`>}
*/
NODISCARD
static bool parse_visibility_member(parserstate *state, rbs_node_list_t *annotations, rbs_node_t **visibility_member) {
  if (annotations->length > 0) {
    set_error(state, state->current_token, true, "annotation cannot be given to visibility members");
    return false;
  }

  rbs_location_t *location = rbs_location_current_token(state);

  switch (state->current_token.type)
  {
  case kPUBLIC: {
    *visibility_member = (rbs_node_t *) rbs_ast_members_public_new(&state->allocator, location);
    return true;
  }
  case kPRIVATE: {
    *visibility_member = (rbs_node_t *) rbs_ast_members_private_new(&state->allocator, location);
    return true;
  }
  default:
    set_error(state, state->current_token, false, "Unexpected error");
    return false;
  }
}

/*
  attribute_member ::= {attr_keyword} attr_name attr_var `:` <type>
                     | {visibility} attr_keyword attr_name attr_var `:` <type>
                     | {attr_keyword} `self` `.` attr_name attr_var `:` <type>
                     | {visibility} attr_keyword `self` `.` attr_name attr_var `:` <type>

  attr_keyword ::= `attr_reader` | `attr_writer` | `attr_accessor`

  visibility ::= `public` | `private`

  attr_var ::=                    # empty
             | `(` tAIDENT `)`    # Ivar name
             | `(` `)`            # No variable
*/
NODISCARD
static bool parse_attribute_member(parserstate *state, position comment_pos, rbs_node_list_t *annotations, rbs_node_t **attribute_member) {
  range member_range;

  member_range.start = state->current_token.range.start;
  comment_pos = rbs_nonnull_pos_or(comment_pos, member_range.start);
  rbs_ast_comment_t *comment = get_comment(state, comment_pos.line);

  range visibility_range;
  rbs_keyword_t *visibility;
  switch (state->current_token.type)
  {
  case kPRIVATE: {
    visibility_range = state->current_token.range;
    visibility = rbs_keyword_new(&state->allocator, rbs_location_new(&state->allocator, visibility_range), INTERN("private"));
    parser_advance(state);
    break;
  }
  case kPUBLIC: {
    visibility_range = state->current_token.range;
    visibility = rbs_keyword_new(&state->allocator, rbs_location_new(&state->allocator, visibility_range), INTERN("public"));
    parser_advance(state);
    break;
  }
  default:
    visibility = NULL;
    visibility_range = NULL_RANGE;
    break;
  }

  enum RBSTokenType attr_type = state->current_token.type;
  range keyword_range = state->current_token.range;

  range kind_range;
  InstanceSingletonKind is_kind = parse_instance_singleton_kind(state, false, &kind_range);

  rbs_keyword_t *kind = rbs_keyword_new(
    &state->allocator,
    rbs_location_new(&state->allocator, keyword_range),
    INTERN(((is_kind == INSTANCE_KIND) ? "instance" : "singleton"))
  );

  range name_range;
  rbs_ast_symbol_t *attr_name;
  CHECK_PARSE(parse_method_name(state, &name_range, &attr_name));

  rbs_node_t *ivar_name; // rbs_ast_symbol_t, NULL or rbs_ast_bool_new(&state->allocator, false)
  range ivar_range, ivar_name_range;
  if (state->next_token.type == pLPAREN) {
    ADVANCE_ASSERT(state, pLPAREN);
    ivar_range.start = state->current_token.range.start;

    if (parser_advance_if(state, tAIDENT)) {
      rbs_location_t *symbolLoc = rbs_location_current_token(state);
      ivar_name = (rbs_node_t *) rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, INTERN_TOKEN(state, state->current_token));
      ivar_name_range = state->current_token.range;
    } else {
      range false_range = {
        .start = state->current_token.range.start,
        .end = state->current_token.range.end
      };
      ivar_name = (rbs_node_t *) rbs_ast_bool_new(&state->allocator, rbs_location_new(&state->allocator, false_range), false);
      ivar_name_range = NULL_RANGE;
    }

    ADVANCE_ASSERT(state, pRPAREN);
    ivar_range.end = state->current_token.range.end;
  } else {
    ivar_range = NULL_RANGE;
    ivar_name = NULL;
    ivar_name_range = NULL_RANGE;
  }

  ADVANCE_ASSERT(state, pCOLON);
  range colon_range = state->current_token.range;

  parser_push_typevar_table(state, is_kind == SINGLETON_KIND);

  rbs_node_t *type;
  CHECK_PARSE(parse_type(state, &type));

  CHECK_PARSE(parser_pop_typevar_table(state));

  member_range.end = state->current_token.range.end;

  rbs_location_t *loc = rbs_location_new(&state->allocator, member_range);
  rbs_loc_alloc_children(&state->allocator, loc, 7);
  rbs_loc_add_required_child(loc, INTERN("keyword"), keyword_range);
  rbs_loc_add_required_child(loc, INTERN("name"), name_range);
  rbs_loc_add_required_child(loc, INTERN("colon"), colon_range);
  rbs_loc_add_optional_child(loc, INTERN("kind"), kind_range);
  rbs_loc_add_optional_child(loc, INTERN("ivar"), ivar_range);
  rbs_loc_add_optional_child(loc, INTERN("ivar_name"), ivar_name_range);
  rbs_loc_add_optional_child(loc, INTERN("visibility"), visibility_range);

  switch (attr_type)
  {
  case kATTRREADER:
    *attribute_member = (rbs_node_t *) rbs_ast_members_attrreader_new(&state->allocator, loc, attr_name, type, ivar_name, kind, annotations, comment, visibility);
    return true;
  case kATTRWRITER:
    *attribute_member = (rbs_node_t *) rbs_ast_members_attrwriter_new(&state->allocator, loc, attr_name, type, ivar_name, kind, annotations, comment, visibility);
    return true;
  case kATTRACCESSOR:
    *attribute_member = (rbs_node_t *) rbs_ast_members_attraccessor_new(&state->allocator, loc, attr_name, type, ivar_name, kind, annotations, comment, visibility);
    return true;
  default:
    set_error(state, state->current_token, false, "Unexpected error");
    return false;
  }
}

/*
  interface_members ::= {} ...<interface_member> kEND

  interface_member ::= def_member     (instance method only && no overloading)
                     | mixin_member   (interface only)
                     | alias_member   (instance only)
*/
NODISCARD
static bool parse_interface_members(parserstate *state, rbs_node_list_t **members) {
  *members = rbs_node_list_new(&state->allocator);

  while (state->next_token.type != kEND) {
    rbs_node_list_t *annotations = rbs_node_list_new(&state->allocator);
    position annot_pos = NullPosition;

    CHECK_PARSE(parse_annotations(state, annotations, &annot_pos));
    parser_advance(state);

    rbs_node_t *member;
    switch (state->current_token.type) {
    case kDEF: {
      rbs_ast_members_methoddefinition_t *method_definition = NULL;
      CHECK_PARSE(parse_member_def(state, true, true, annot_pos, annotations, &method_definition));
      member = (rbs_node_t *) method_definition;
      break;
    }

    case kINCLUDE:
    case kEXTEND:
    case kPREPEND: {
      CHECK_PARSE(parse_mixin_member(state, true, annot_pos, annotations, &member));
      break;
    }

    case kALIAS: {
      rbs_ast_members_alias_t *alias_member = NULL;
      CHECK_PARSE(parse_alias_member(state, true, annot_pos, annotations, &alias_member));
      member = (rbs_node_t *) alias_member;
      break;
    }

    default:
      set_error(state, state->current_token, true, "unexpected token for interface declaration member");
      return false;
    }

    rbs_node_list_append(*members, member);
  }

  return true;
}

/*
  interface_decl ::= {`interface`} interface_name module_type_params interface_members <kEND>
*/
NODISCARD
static bool parse_interface_decl(parserstate *state, position comment_pos, rbs_node_list_t *annotations, rbs_ast_declarations_interface_t **interface_decl) {
  parser_push_typevar_table(state, true);

  range member_range;
  member_range.start = state->current_token.range.start;
  comment_pos = rbs_nonnull_pos_or(comment_pos, member_range.start);

  range keyword_range = state->current_token.range;

  parser_advance(state);

  range name_range;
  rbs_typename_t *name = NULL;
  CHECK_PARSE(parse_type_name(state, INTERFACE_NAME, &name_range, &name));

  range type_params_range;
  rbs_node_list_t *type_params;
  CHECK_PARSE(parse_type_params(state, &type_params_range, true, &type_params));

  rbs_node_list_t *members = NULL;
  CHECK_PARSE(parse_interface_members(state, &members));

  ADVANCE_ASSERT(state, kEND);
  range end_range = state->current_token.range;
  member_range.end = end_range.end;

  CHECK_PARSE(parser_pop_typevar_table(state));

  rbs_location_t *loc = rbs_location_new(&state->allocator, member_range);
  rbs_loc_alloc_children(&state->allocator, loc, 4);
  rbs_loc_add_required_child(loc, INTERN("keyword"), keyword_range);
  rbs_loc_add_required_child(loc, INTERN("name"), name_range);
  rbs_loc_add_required_child(loc, INTERN("end"), end_range);
  rbs_loc_add_optional_child(loc, INTERN("type_params"), type_params_range);

  rbs_ast_comment_t *comment = get_comment(state, comment_pos.line);

  *interface_decl = rbs_ast_declarations_interface_new(&state->allocator, loc, name, type_params, members, annotations, comment);
  return true;
}

/*
  module_self_types ::= {`:`} module_self_type `,` ... `,` <module_self_type>

  module_self_type ::= <module_name>
                     | module_name `[` type_list <`]`>
*/
NODISCARD
static bool parse_module_self_types(parserstate *state, rbs_node_list_t *array) {
  while (true) {
    parser_advance(state);

    range self_range;
    self_range.start = state->current_token.range.start;

    range name_range;
    rbs_typename_t *module_name = NULL;
    CHECK_PARSE(parse_type_name(state, CLASS_NAME | INTERFACE_NAME, &name_range, &module_name));
    self_range.end = name_range.end;

    rbs_node_list_t *args = rbs_node_list_new(&state->allocator);
    range args_range = NULL_RANGE;
    if (state->next_token.type == pLBRACKET) {
      parser_advance(state);
      args_range.start = state->current_token.range.start;
      CHECK_PARSE(parse_type_list(state, pRBRACKET, args));
      parser_advance(state);
      self_range.end = args_range.end = state->current_token.range.end;
    }

    rbs_location_t *loc = rbs_location_new(&state->allocator, self_range);
    rbs_loc_alloc_children(&state->allocator, loc, 2);
    rbs_loc_add_required_child(loc, INTERN("name"), name_range);
    rbs_loc_add_optional_child(loc, INTERN("args"), args_range);

    rbs_ast_declarations_module_self_t *self_type = rbs_ast_declarations_module_self_new(&state->allocator, loc, module_name, args);
    rbs_node_list_append(array, (rbs_node_t *)self_type);

    if (state->next_token.type == pCOMMA) {
      parser_advance(state);
    } else {
      break;
    }
  }

  return true;
}

NODISCARD
static bool parse_nested_decl(parserstate *state, const char *nested_in, position annot_pos, rbs_node_list_t *annotations, rbs_node_t **decl);

/*
  module_members ::= {} ...<module_member> kEND

  module_member ::= def_member
                  | variable_member
                  | mixin_member
                  | alias_member
                  | attribute_member
                  | `public`
                  | `private`
*/
NODISCARD
static bool parse_module_members(parserstate *state, rbs_node_list_t **members) {
  *members = rbs_node_list_new(&state->allocator);

  while (state->next_token.type != kEND) {
    rbs_node_list_t *annotations = rbs_node_list_new(&state->allocator);
    position annot_pos;
    CHECK_PARSE(parse_annotations(state, annotations, &annot_pos));

    parser_advance(state);

    rbs_node_t *member;
    switch (state->current_token.type)
    {
    case kDEF: {
      rbs_ast_members_methoddefinition_t *method_definition;
      CHECK_PARSE(parse_member_def(state, false, true, annot_pos, annotations, &method_definition));
      member = (rbs_node_t *) method_definition;
      break;
    }

    case kINCLUDE:
    case kEXTEND:
    case kPREPEND: {
      CHECK_PARSE(parse_mixin_member(state, false, annot_pos, annotations, &member));
      break;
    }
    case kALIAS: {
      rbs_ast_members_alias_t *alias_member = NULL;
      CHECK_PARSE(parse_alias_member(state, false, annot_pos, annotations, &alias_member));
      member = (rbs_node_t *) alias_member;
      break;
    }
    case tAIDENT:
    case tA2IDENT:
    case kSELF: {
      CHECK_PARSE(parse_variable_member(state, annot_pos, annotations, &member));
      break;
    }

    case kATTRREADER:
    case kATTRWRITER:
    case kATTRACCESSOR: {
      CHECK_PARSE(parse_attribute_member(state, annot_pos, annotations, &member));
      break;
    }

    case kPUBLIC:
    case kPRIVATE:
      if (state->next_token.range.start.line == state->current_token.range.start.line) {
        switch (state->next_token.type)
        {
        case kDEF: {
          rbs_ast_members_methoddefinition_t *method_definition = NULL;
          CHECK_PARSE(parse_member_def(state, false, true, annot_pos, annotations, &method_definition));
          member = (rbs_node_t *) method_definition;
          break;
        }
        case kATTRREADER:
        case kATTRWRITER:
        case kATTRACCESSOR: {
          CHECK_PARSE(parse_attribute_member(state, annot_pos, annotations, &member));
          break;
        }
        default:
          set_error(state, state->next_token, true, "method or attribute definition is expected after visibility modifier");
          return false;
        }
      } else {
        CHECK_PARSE(parse_visibility_member(state, annotations, &member));
      }
      break;

    default:
      CHECK_PARSE(parse_nested_decl(state, "module", annot_pos, annotations, &member));
      break;
    }

    rbs_node_list_append(*members, member);
  }

  return true;
}

/*
  module_decl ::= {module_name} module_type_params module_members <kEND>
                | {module_name} module_name module_type_params `:` module_self_types module_members <kEND>
*/
NODISCARD
static bool parse_module_decl0(parserstate *state, range keyword_range, rbs_typename_t *module_name, range name_range, rbs_ast_comment_t *comment, rbs_node_list_t *annotations, rbs_ast_declarations_module_t **module_decl) {
  parser_push_typevar_table(state, true);

  range decl_range;
  decl_range.start = keyword_range.start;

  range type_params_range;
  rbs_node_list_t *type_params;
  CHECK_PARSE(parse_type_params(state, &type_params_range, true, &type_params));

  rbs_node_list_t *self_types = rbs_node_list_new(&state->allocator);
  range colon_range;
  range self_types_range;
  if (state->next_token.type == pCOLON) {
    parser_advance(state);
    colon_range = state->current_token.range;
    self_types_range.start = state->next_token.range.start;
    CHECK_PARSE(parse_module_self_types(state, self_types));
    self_types_range.end = state->current_token.range.end;
  } else {
    colon_range = NULL_RANGE;
    self_types_range = NULL_RANGE;
  }

  rbs_node_list_t *members = NULL;
  CHECK_PARSE(parse_module_members(state, &members));

  ADVANCE_ASSERT(state, kEND);
  range end_range = state->current_token.range;
  decl_range.end = state->current_token.range.end;

  rbs_location_t *loc = rbs_location_new(&state->allocator, decl_range);
  rbs_loc_alloc_children(&state->allocator, loc, 6);
  rbs_loc_add_required_child(loc, INTERN("keyword"), keyword_range);
  rbs_loc_add_required_child(loc, INTERN("name"), name_range);
  rbs_loc_add_required_child(loc, INTERN("end"), end_range);
  rbs_loc_add_optional_child(loc, INTERN("type_params"), type_params_range);
  rbs_loc_add_optional_child(loc, INTERN("colon"), colon_range);
  rbs_loc_add_optional_child(loc, INTERN("self_types"), self_types_range);

  CHECK_PARSE(parser_pop_typevar_table(state));

  *module_decl = rbs_ast_declarations_module_new(&state->allocator, loc, module_name, type_params, self_types, members, annotations, comment);
  return true;
}

/*
  module_decl ::= {`module`} module_name `=` old_module_name <kEND>
                | {`module`} module_name module_decl0 <kEND>

*/
NODISCARD
static bool parse_module_decl(parserstate *state, position comment_pos, rbs_node_list_t *annotations, rbs_node_t **module_decl) {
  range keyword_range = state->current_token.range;

  comment_pos = rbs_nonnull_pos_or(comment_pos, state->current_token.range.start);
  rbs_ast_comment_t *comment = get_comment(state, comment_pos.line);

  parser_advance(state);

  range module_name_range;
  rbs_typename_t *module_name;
  CHECK_PARSE(parse_type_name(state, CLASS_NAME, &module_name_range, &module_name));

  if (state->next_token.type == pEQ) {
    range eq_range = state->next_token.range;
    parser_advance(state);
    parser_advance(state);

    range old_name_range;
    rbs_typename_t *old_name = NULL;
    CHECK_PARSE(parse_type_name(state, CLASS_NAME, &old_name_range, &old_name));

    range decl_range = {
      .start = keyword_range.start,
      .end = old_name_range.end
    };

    rbs_location_t *loc = rbs_location_new(&state->allocator, decl_range);
    rbs_loc_alloc_children(&state->allocator, loc, 4);
    rbs_loc_add_required_child(loc, INTERN("keyword"), keyword_range);
    rbs_loc_add_required_child(loc, INTERN("new_name"), module_name_range);
    rbs_loc_add_required_child(loc, INTERN("eq"), eq_range);
    rbs_loc_add_optional_child(loc, INTERN("old_name"), old_name_range);

    *module_decl = (rbs_node_t *) rbs_ast_declarations_modulealias_new(&state->allocator, loc, module_name, old_name, comment, annotations);
  } else {
    rbs_ast_declarations_module_t *module_decl0 = NULL;
    CHECK_PARSE(parse_module_decl0(state, keyword_range, module_name, module_name_range, comment, annotations, &module_decl0));
    *module_decl = (rbs_node_t *) module_decl0;
  }

  return true;
}

/*
  class_decl_super ::= {} `<` <class_instance_name>
                     | {<>}
*/
NODISCARD
static bool parse_class_decl_super(parserstate *state, range *lt_range, rbs_ast_declarations_class_super_t **super) {
  if (parser_advance_if(state, pLT)) {
    *lt_range = state->current_token.range;

    range super_range;
    super_range.start = state->next_token.range.start;

    rbs_node_list_t *args = rbs_node_list_new(&state->allocator);
    rbs_typename_t *name = NULL;
    range name_range, args_range;
    CHECK_PARSE(class_instance_name(state, CLASS_NAME, args, &name_range, &args_range, &name));

    super_range.end = state->current_token.range.end;

    rbs_location_t *loc = rbs_location_new(&state->allocator, super_range);
    rbs_loc_alloc_children(&state->allocator, loc, 2);
    rbs_loc_add_required_child(loc, INTERN("name"), name_range);
    rbs_loc_add_optional_child(loc, INTERN("args"), args_range);


    *super = rbs_ast_declarations_class_super_new(&state->allocator, loc, name, args);
  } else {
    *lt_range = NULL_RANGE;
  }

  return true;
}

/*
  class_decl ::= {class_name} type_params class_decl_super class_members <`end`>
*/
NODISCARD
static bool parse_class_decl0(parserstate *state, range keyword_range, rbs_typename_t *name, range name_range, rbs_ast_comment_t *comment, rbs_node_list_t *annotations, rbs_ast_declarations_class_t **class_decl) {
  parser_push_typevar_table(state, true);

  range decl_range;
  decl_range.start = keyword_range.start;

  range type_params_range;
  rbs_node_list_t *type_params;
  CHECK_PARSE(parse_type_params(state, &type_params_range, true, &type_params));

  range lt_range;
  rbs_ast_declarations_class_super_t *super = NULL;
  CHECK_PARSE(parse_class_decl_super(state, &lt_range, &super));

  rbs_node_list_t *members = NULL;
  CHECK_PARSE(parse_module_members(state, &members));

  ADVANCE_ASSERT(state, kEND);

  range end_range = state->current_token.range;

  decl_range.end = end_range.end;

  CHECK_PARSE(parser_pop_typevar_table(state));

  rbs_location_t *loc = rbs_location_new(&state->allocator, decl_range);
  rbs_loc_alloc_children(&state->allocator, loc, 5);
  rbs_loc_add_required_child(loc, INTERN("keyword"), keyword_range);
  rbs_loc_add_required_child(loc, INTERN("name"), name_range);
  rbs_loc_add_required_child(loc, INTERN("end"), end_range);
  rbs_loc_add_optional_child(loc, INTERN("type_params"), type_params_range);
  rbs_loc_add_optional_child(loc, INTERN("lt"), lt_range);

  *class_decl = rbs_ast_declarations_class_new(&state->allocator, loc, name, type_params, super, members, annotations, comment);
  return true;
}

/*
  class_decl ::= {`class`} class_name `=` <class_name>
               | {`class`} class_name <class_decl0>
*/
NODISCARD
static bool parse_class_decl(parserstate *state, position comment_pos, rbs_node_list_t *annotations, rbs_node_t **class_decl) {
  range keyword_range = state->current_token.range;

  comment_pos = rbs_nonnull_pos_or(comment_pos, state->current_token.range.start);
  rbs_ast_comment_t *comment = get_comment(state, comment_pos.line);

  parser_advance(state);
  range class_name_range;
  rbs_typename_t *class_name = NULL;
  CHECK_PARSE(parse_type_name(state, CLASS_NAME, &class_name_range, &class_name));

  if (state->next_token.type == pEQ) {
    range eq_range = state->next_token.range;
    parser_advance(state);
    parser_advance(state);

    range old_name_range;
    rbs_typename_t *old_name = NULL;
    CHECK_PARSE(parse_type_name(state, CLASS_NAME, &old_name_range, &old_name));

    range decl_range = {
      .start = keyword_range.start,
      .end = old_name_range.end,
    };

    rbs_location_t *loc = rbs_location_new(&state->allocator, decl_range);
    rbs_loc_alloc_children(&state->allocator, loc, 4);
    rbs_loc_add_required_child(loc, INTERN("keyword"), keyword_range);
    rbs_loc_add_required_child(loc, INTERN("new_name"), class_name_range);
    rbs_loc_add_required_child(loc, INTERN("eq"), eq_range);
    rbs_loc_add_optional_child(loc, INTERN("old_name"), old_name_range);

    *class_decl = (rbs_node_t *) rbs_ast_declarations_classalias_new(&state->allocator, loc, class_name, old_name, comment, annotations);
  } else {
    rbs_ast_declarations_class_t *class_decl0 = NULL;
    CHECK_PARSE(parse_class_decl0(state, keyword_range, class_name, class_name_range, comment, annotations, &class_decl0));
    *class_decl = (rbs_node_t *) class_decl0;
  }

  return true;
}

/*
  nested_decl ::= {<const_decl>}
                | {<class_decl>}
                | {<interface_decl>}
                | {<module_decl>}
                | {<class_decl>}
*/
NODISCARD
static bool parse_nested_decl(parserstate *state, const char *nested_in, position annot_pos, rbs_node_list_t *annotations, rbs_node_t **decl) {
  parser_push_typevar_table(state, true);

  switch (state->current_token.type) {
  case tUIDENT:
  case pCOLON2: {
    rbs_ast_declarations_constant_t *constant = NULL;
    CHECK_PARSE(parse_const_decl(state, annotations, &constant));
    *decl = (rbs_node_t *) constant;
    break;
  }
  case tGIDENT: {
    rbs_ast_declarations_global_t *global = NULL;
    CHECK_PARSE(parse_global_decl(state, annotations, &global));
    *decl = (rbs_node_t *) global;
    break;
  }
  case kTYPE: {
    rbs_ast_declarations_typealias_t *typealias = NULL;
    CHECK_PARSE(parse_type_decl(state, annot_pos, annotations, &typealias));
    *decl = (rbs_node_t *) typealias;
    break;
  }
  case kINTERFACE: {
    rbs_ast_declarations_interface_t *interface_decl = NULL;
    CHECK_PARSE(parse_interface_decl(state, annot_pos, annotations, &interface_decl));
    *decl = (rbs_node_t *) interface_decl;
    break;
  }
  case kMODULE: {
    rbs_node_t *module_decl = NULL;
    CHECK_PARSE(parse_module_decl(state, annot_pos, annotations, &module_decl));
    *decl = module_decl;
    break;
  }
  case kCLASS: {
    rbs_node_t *class_decl = NULL;
    CHECK_PARSE(parse_class_decl(state, annot_pos, annotations, &class_decl));
    *decl = class_decl;
    break;
  }
  default:
    set_error(state, state->current_token, true, "unexpected token for class/module declaration member");
    return false;
  }

  CHECK_PARSE(parser_pop_typevar_table(state));

  return true;
}

NODISCARD
static bool parse_decl(parserstate *state, rbs_node_t **decl) {
  rbs_node_list_t *annotations = rbs_node_list_new(&state->allocator);
  position annot_pos = NullPosition;

  CHECK_PARSE(parse_annotations(state, annotations, &annot_pos));
  parser_advance(state);

  switch (state->current_token.type) {
  case tUIDENT:
  case pCOLON2: {
    rbs_ast_declarations_constant_t *constant = NULL;
    CHECK_PARSE(parse_const_decl(state, annotations, &constant));
    *decl = (rbs_node_t *) constant;
    return true;
  }
  case tGIDENT: {
    rbs_ast_declarations_global_t *global = NULL;
    CHECK_PARSE(parse_global_decl(state, annotations, &global));
    *decl = (rbs_node_t *) global;
    return true;
  }
  case kTYPE: {
    rbs_ast_declarations_typealias_t *typealias = NULL;
    CHECK_PARSE(parse_type_decl(state, annot_pos, annotations, &typealias));
    *decl = (rbs_node_t *) typealias;
    return true;
  }
  case kINTERFACE: {
    rbs_ast_declarations_interface_t *interface_decl = NULL;
    CHECK_PARSE(parse_interface_decl(state, annot_pos, annotations, &interface_decl));
    *decl = (rbs_node_t *) interface_decl;
    return true;
  }
  case kMODULE: {
    rbs_node_t *module_decl = NULL;
    CHECK_PARSE(parse_module_decl(state, annot_pos, annotations, &module_decl));
    *decl = module_decl;
    return true;
  }
  case kCLASS: {
    rbs_node_t *class_decl = NULL;
    CHECK_PARSE(parse_class_decl(state, annot_pos, annotations, &class_decl));
    *decl = class_decl;
    return true;
  }
  default:
    set_error(state, state->current_token, true, "cannot start a declaration");
    return false;
  }
}

/*
  namespace ::= {} (`::`)? (`tUIDENT` `::`)* `tUIDENT` <`::`>
              | {} <>                                            (empty -- returns empty namespace)
*/
NODISCARD
static bool parse_namespace(parserstate *state, range *rg, rbs_namespace_t **namespace) {
  bool is_absolute = false;

  if (state->next_token.type == pCOLON2) {
    *rg = (range) {
      .start = state->next_token.range.start,
      .end = state->next_token.range.end,
    };
    is_absolute = true;

    parser_advance(state);
  }

  rbs_node_list_t *path = rbs_node_list_new(&state->allocator);

  while (true) {
    if (state->next_token.type == tUIDENT && state->next_token2.type == pCOLON2) {
      rbs_location_t *symbolLoc = rbs_location_new(&state->allocator, state->next_token.range);
      rbs_ast_symbol_t *symbol = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, INTERN_TOKEN(state, state->next_token));
      rbs_node_list_append(path, (rbs_node_t *)symbol);
      if (rbs_null_position_p(rg->start)) {
        rg->start = state->next_token.range.start;
      }
      rg->end = state->next_token2.range.end;
      parser_advance(state);
      parser_advance(state);
    } else {
      break;
    }
  }

  *namespace = rbs_namespace_new(&state->allocator, rbs_location_new(&state->allocator, *rg), path, is_absolute);
  return true;
}

/*
  use_clauses ::= {} use_clause `,` ... `,` <use_clause>

  use_clause ::= {} namespace <tUIDENT>
               | {} namespace tUIDENT `as` <tUIDENT>
               | {} namespace <tSTAR>
*/
NODISCARD
static bool parse_use_clauses(parserstate *state, rbs_node_list_t *clauses) {
  while (true) {
    range namespace_range = NULL_RANGE;
    rbs_namespace_t *namespace = NULL;
    CHECK_PARSE(parse_namespace(state, &namespace_range, &namespace));

    switch (state->next_token.type)
    {
      case tLIDENT:
      case tULIDENT:
      case tUIDENT: {
        parser_advance(state);

        enum RBSTokenType ident_type = state->current_token.type;

        range type_name_range = rbs_null_range_p(namespace_range)
          ? state->current_token.range
          : (range) { .start = namespace_range.start, .end = state->current_token.range.end };

        rbs_location_t *symbolLoc = rbs_location_current_token(state);
        rbs_ast_symbol_t *symbol = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, INTERN_TOKEN(state, state->current_token));
        rbs_typename_t *type_name = rbs_typename_new(&state->allocator, rbs_location_new(&state->allocator, type_name_range),namespace, symbol);

        range keyword_range = NULL_RANGE;
        range new_name_range = NULL_RANGE;
        rbs_ast_symbol_t *new_name = NULL;
        range clause_range = type_name_range;
        if (state->next_token.type == kAS) {
          parser_advance(state);
          keyword_range = state->current_token.range;

          if (ident_type == tUIDENT) ADVANCE_ASSERT(state, tUIDENT);
          if (ident_type == tLIDENT) ADVANCE_ASSERT(state, tLIDENT);
          if (ident_type == tULIDENT) ADVANCE_ASSERT(state, tULIDENT);

          rbs_location_t *symbolLoc = rbs_location_new(&state->allocator, new_name_range);
          new_name = rbs_ast_symbol_new(&state->allocator, symbolLoc, &state->constant_pool, INTERN_TOKEN(state, state->current_token));
          new_name_range = state->current_token.range;
          clause_range.end = new_name_range.end;
        }

        rbs_location_t *loc = rbs_location_new(&state->allocator, clause_range);
        rbs_loc_alloc_children(&state->allocator, loc, 3);
        rbs_loc_add_required_child(loc, INTERN("type_name"), type_name_range);
        rbs_loc_add_optional_child(loc, INTERN("keyword"), keyword_range);
        rbs_loc_add_optional_child(loc, INTERN("new_name"), new_name_range);

        rbs_ast_directives_use_singleclause_t *clause = rbs_ast_directives_use_singleclause_new(&state->allocator, loc, type_name, new_name);
        rbs_node_list_append(clauses, (rbs_node_t *)clause);

        break;
      }
      case pSTAR:
      {
        range clause_range = namespace_range;
        parser_advance(state);

        range star_range = state->current_token.range;
        clause_range.end = star_range.end;

        rbs_location_t *loc = rbs_location_new(&state->allocator, clause_range);
        rbs_loc_alloc_children(&state->allocator, loc, 2);
        rbs_loc_add_required_child(loc, INTERN("namespace"), namespace_range);
        rbs_loc_add_required_child(loc, INTERN("star"), star_range);

        rbs_ast_directives_use_wildcardclause_t *clause = rbs_ast_directives_use_wildcardclause_new(&state->allocator, loc, namespace);
        rbs_node_list_append(clauses, (rbs_node_t *)clause);

        break;
      }
      default:
        set_error(state, state->next_token, true, "use clause is expected");
        return false;
    }

    if (state->next_token.type == pCOMMA) {
      parser_advance(state);
    } else {
      break;
    }
  }

  return true;
}

/*
  use_directive ::= {} `use` <clauses>
 */
NODISCARD
static bool parse_use_directive(parserstate *state, rbs_ast_directives_use_t **use_directive) {
  if (state->next_token.type == kUSE) {
    parser_advance(state);

    range keyword_range = state->current_token.range;

    rbs_node_list_t *clauses = rbs_node_list_new(&state->allocator);
    CHECK_PARSE(parse_use_clauses(state, clauses));

    range directive_range = keyword_range;
    directive_range.end = state->current_token.range.end;

    rbs_location_t *loc = rbs_location_new(&state->allocator, directive_range);
    rbs_loc_alloc_children(&state->allocator, loc, 1);
    rbs_loc_add_required_child(loc, INTERN("keyword"), keyword_range);

    *use_directive = rbs_ast_directives_use_new(&state->allocator, loc, clauses);
  }

  return true;
}

static rbs_ast_comment_t *parse_comment_lines(parserstate *state, comment *com) {
  size_t hash_bytes = state->lexstate->encoding->char_width((const uint8_t *) "#", (size_t) 1);
  size_t space_bytes = state->lexstate->encoding->char_width((const uint8_t *) " ", (size_t) 1);

  rbs_buffer_t rbs_buffer;
  rbs_buffer_init(&state->allocator, &rbs_buffer);

  for (size_t i = 0; i < com->line_count; i++) {
    token tok = com->tokens[i];

    const char *comment_start = state->lexstate->string.start + tok.range.start.byte_pos + hash_bytes;
    size_t comment_bytes = RBS_RANGE_BYTES(tok.range) - hash_bytes;

    rbs_string_t str = rbs_string_new(
      comment_start,
      state->lexstate->string.end
    );
    unsigned char c = rbs_utf8_to_codepoint(str);

    if (c == ' ') {
      comment_start += space_bytes;
      comment_bytes -= space_bytes;
    }

    rbs_buffer_append_string(&state->allocator, &rbs_buffer, comment_start, comment_bytes);
    rbs_buffer_append_cstr(&state->allocator, &rbs_buffer, "\n");
  }

  return rbs_ast_comment_new(
    &state->allocator,
    rbs_location_new(&state->allocator, (range) { .start = com->start, .end = com->end }),
    rbs_buffer_to_string(&rbs_buffer)
  );
}

/**
 * Insert new comment line token.
 * */
static void insert_comment_line(parserstate *state, token tok) {
  int prev_line = tok.range.start.line - 1;

  comment *com = comment_get_comment(state->last_comment, prev_line);

  if (com) {
    comment_insert_new_line(&state->allocator, com, tok);
  } else {
    state->last_comment = alloc_comment(&state->allocator, tok, state->last_comment);
  }
}

bool parse_signature(parserstate *state, rbs_signature_t **signature) {
  range signature_range;
  signature_range.start = state->current_token.range.start;

  rbs_node_list_t *dirs = rbs_node_list_new(&state->allocator);
  rbs_node_list_t *decls = rbs_node_list_new(&state->allocator);

  while (state->next_token.type == kUSE) {
    rbs_ast_directives_use_t *use_node;
    CHECK_PARSE(parse_use_directive(state, &use_node));

    if (use_node == NULL) {
      rbs_node_list_append(dirs, NULL);
    } else {
      rbs_node_list_append(dirs, (rbs_node_t *)use_node);
    }
  }

  while (state->next_token.type != pEOF) {
    rbs_node_t *decl = NULL;
    CHECK_PARSE(parse_decl(state, &decl));
    rbs_node_list_append(decls, decl);
  }

  signature_range.end = state->current_token.range.end;
  *signature = rbs_signature_new(&state->allocator, rbs_location_new(&state->allocator, signature_range), dirs, decls);
  return true;
}

#define RESET_TABLE_P(table) (table->size == 0)

id_table *alloc_empty_table(rbs_allocator_t *allocator) {
  id_table *table = rbs_allocator_alloc(allocator, id_table);

  *table = (id_table) {
    .size = 10,
    .count = 0,
    .ids = rbs_allocator_calloc(allocator, 10, rbs_constant_id_t),
    .next = NULL,
  };

  return table;
}

id_table *alloc_reset_table(rbs_allocator_t *allocator) {
  id_table *table = rbs_allocator_alloc(allocator, id_table);

  *table = (id_table) {
    .size = 0,
    .count = 0,
    .ids = NULL,
    .next = NULL,
  };

  return table;
}

id_table *parser_push_typevar_table(parserstate *state, bool reset) {
  if (reset) {
    id_table *table = alloc_reset_table(&state->allocator);
    table->next = state->vars;
    state->vars = table;
  }

  id_table *table = alloc_empty_table(&state->allocator);
  table->next = state->vars;
  state->vars = table;

  return table;
}

NODISCARD
bool parser_pop_typevar_table(parserstate *state) {
  id_table *table;

  if (state->vars) {
    table = state->vars;
    state->vars = table->next;
  } else {
    set_error(state, state->current_token, false, "Cannot pop empty table");
    return false;
  }

  if (state->vars && RESET_TABLE_P(state->vars)) {
    table = state->vars;
    state->vars = table->next;
  }

  return true;
}

NODISCARD
bool parser_insert_typevar(parserstate *state, rbs_constant_id_t id) {
  id_table *table = state->vars;

  if (RESET_TABLE_P(table)) {
    set_error(state, state->current_token, false, "Cannot insert to reset table");
    return false;
  }

  if (table->size == table->count) {
    // expand
    rbs_constant_id_t *ptr = table->ids;
    table->size += 10;
    table->ids = rbs_allocator_calloc(&state->allocator, table->size, rbs_constant_id_t);
    memcpy(table->ids, ptr, sizeof(rbs_constant_id_t) * table->count);
  }

  table->ids[table->count++] = id;

  return true;
}

bool parser_typevar_member(parserstate *state, rbs_constant_id_t id) {
  id_table *table = state->vars;

  while (table && !RESET_TABLE_P(table)) {
    for (size_t i = 0; i < table->count; i++) {
      if (table->ids[i] == id) {
        return true;
      }
    }

    table = table->next;
  }

  return false;
}

void print_parser(parserstate *state) {
  printf("  current_token = %s (%d...%d)\n", token_type_str(state->current_token.type), state->current_token.range.start.char_pos, state->current_token.range.end.char_pos);
  printf("     next_token = %s (%d...%d)\n", token_type_str(state->next_token.type), state->next_token.range.start.char_pos, state->next_token.range.end.char_pos);
  printf("    next_token2 = %s (%d...%d)\n", token_type_str(state->next_token2.type), state->next_token2.range.start.char_pos, state->next_token2.range.end.char_pos);
  printf("    next_token3 = %s (%d...%d)\n", token_type_str(state->next_token3.type), state->next_token3.range.start.char_pos, state->next_token3.range.end.char_pos);
}

void parser_advance(parserstate *state) {
  state->current_token = state->next_token;
  state->next_token = state->next_token2;
  state->next_token2 = state->next_token3;

  while (true) {
    if (state->next_token3.type == pEOF) {
      break;
    }

    state->next_token3 = rbsparser_next_token(state->lexstate);

    if (state->next_token3.type == tCOMMENT) {
      // skip
    } else if (state->next_token3.type == tLINECOMMENT) {
      insert_comment_line(state, state->next_token3);
    } else if (state->next_token3.type == tTRIVIA) {
      //skip
    } else {
      break;
    }
  }
}

void rbs_print_token(token tok) {
  printf(
    "%s char=%d...%d\n",
    token_type_str(tok.type),
    tok.range.start.char_pos,
    tok.range.end.char_pos
  );
}

rbs_ast_comment_t *get_comment(parserstate *state, int subject_line) {
  int comment_line = subject_line - 1;

  comment *com = comment_get_comment(state->last_comment, comment_line);

  if (com) {
    return parse_comment_lines(state, com);
  } else {
    return NULL;
  }
}

comment *alloc_comment(rbs_allocator_t *allocator, token comment_token, comment *last_comment) {
  comment *new_comment = rbs_allocator_alloc(allocator, comment);

  *new_comment = (comment) {
    .start = comment_token.range.start,
    .end = comment_token.range.end,

    .line_size = 0,
    .line_count = 0,
    .tokens = NULL,

    .next_comment = last_comment,
  };

  comment_insert_new_line(allocator, new_comment, comment_token);

  return new_comment;
}

void comment_insert_new_line(rbs_allocator_t *allocator, comment *com, token comment_token) {
  if (com->line_count == 0) {
    com->start = comment_token.range.start;
  }

  if (com->line_count == com->line_size) {
    com->line_size += 10;

    if (com->tokens) {
      token *p = com->tokens;
      com->tokens = rbs_allocator_calloc(allocator, com->line_size, token);
      memcpy(com->tokens, p, sizeof(token) * com->line_count);
    } else {
      com->tokens = rbs_allocator_calloc(allocator, com->line_size, token);
    }
  }

  com->tokens[com->line_count++] = comment_token;
  com->end = comment_token.range.end;
}

comment *comment_get_comment(comment *com, int line) {
  if (com == NULL) {
    return NULL;
  }

  if (com->end.line < line) {
    return NULL;
  }

  if (com->end.line == line) {
    return com;
  }

  return comment_get_comment(com->next_comment, line);
}

lexstate *alloc_lexer(rbs_allocator_t *allocator, rbs_string_t string, const rbs_encoding_t *encoding, int start_pos, int end_pos) {
  lexstate *lexer = rbs_allocator_alloc(allocator, lexstate);

  position start_position = (position) {
    .byte_pos = 0,
    .char_pos = 0,
    .line = 1,
    .column = 0,
  };

  *lexer = (lexstate) {
    .string = string,
    .start_pos = start_pos,
    .end_pos = end_pos,
    .current = start_position,
    .start = { 0 },
    .first_token_of_line = false,
    .last_char = 0,
    .encoding = encoding,
  };

  rbs_skipn(lexer, start_pos);
  lexer->start = lexer->current;
  lexer->first_token_of_line = lexer->current.column == 0;

  return lexer;
}

parserstate *alloc_parser(rbs_string_t string, const rbs_encoding_t *encoding, int start_pos, int end_pos) {
  rbs_allocator_t allocator;
  rbs_allocator_init(&allocator);

  lexstate *lexer = alloc_lexer(&allocator, string, encoding, start_pos, end_pos);
  parserstate *parser = rbs_allocator_alloc(&allocator, parserstate);

  *parser = (parserstate) {
    .lexstate = lexer,

    .current_token = NullToken,
    .next_token = NullToken,
    .next_token2 = NullToken,
    .next_token3 = NullToken,

    .vars = NULL,
    .last_comment = NULL,

    .constant_pool = {},
    .allocator = allocator,
    .error = NULL,
  };

  // The parser's constant pool is mainly used for storing the names of type variables, which usually aren't many.
  // Below are some statistics gathered from the current test suite. We can see that 56% of parsers never add to their
  // constant pool at all. The initial capacity needs to be a power of 2. Picking 2 means that we won't need to realloc
  // in 85% of cases.
  //
  // TODO: recalculate these statistics based on a real world codebase, rather than the test suite.
  //
  // | Size | Count | Cumulative | % Coverage |
  // |------|-------|------------|------------|
  // |   0  | 7,862 |      7,862 |     56%    |
  // |   1  | 3,196 |     11,058 |     79%    |
  // |   2  |   778 |     12,719 |     85%    |
  // |   3  |   883 |     11,941 |     91%    |
  // |   4  |   478 |     13,197 |     95%    |
  // |   5  |   316 |     13,513 |     97%    |
  // |   6  |   288 |     13,801 |     99%    |
  // |   7  |   144 |     13,945 |    100%    |
  const size_t initial_pool_capacity = 2;
  rbs_constant_pool_init(&parser->constant_pool, initial_pool_capacity);

  parser_advance(parser);
  parser_advance(parser);
  parser_advance(parser);

  return parser;
}

void free_parser(parserstate *parser) {
  rbs_constant_pool_free(&parser->constant_pool);
  rbs_allocator_free(&parser->allocator);
}

void rbs_parser_declare_type_variables(parserstate *parser, size_t count, const char **variables) {
  if (variables == NULL) return; // Nothing to do.

  parser_push_typevar_table(parser, true);

  for (size_t i = 0; i < count; i++) {
    rbs_constant_id_t name = rbs_constant_pool_insert_shared(
      &parser->constant_pool,
      (const uint8_t *) variables[i],
      strlen(variables[i])
    );

    if (!parser_insert_typevar(parser, name)) {
      rbs_assert(false, "Failed to insert type variable. Message: %s", parser->error->message);
    }
  }
}

void set_error(parserstate *state, token tok, bool syntax_error, const char *fmt, ...) {
  if (state->error) {
    return;
  }

  va_list args;

  va_start(args, fmt);
  int length = vsnprintf(NULL, 0, fmt, args);
  va_end(args);

  char *message = rbs_allocator_alloc_many(&state->allocator, length + 1, char);

  va_start(args, fmt);
  vsnprintf(message, length + 1, fmt, args);
  va_end(args);

  state->error = rbs_allocator_alloc(&state->allocator, error);
  state->error->token = tok;
  state->error->message = message;
  state->error->syntax_error = syntax_error;
}
