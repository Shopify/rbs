#include "rbs_extension.h"

#define INTERN_TOKEN(parserstate, tok) \
  rb_intern3(\
    peek_token(parserstate->lexstate, tok),\
    token_bytes(tok),\
    rb_enc_get(parserstate->lexstate->string)\
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

typedef struct {
  rbs_node_list_t *required_positionals;
  rbs_node_list_t *optional_positionals;
  VALUE rest_positionals;
  rbs_node_list_t *trailing_positionals;
  VALUE required_keywords;
  VALUE optional_keywords;
  VALUE rest_keywords;
} method_params;

static bool rbs_is_untyped_params(method_params *params) {
  return params->required_positionals == NULL;
}

// /**
//  * Returns RBS::Location object of `current_token` of a parser state.
//  *
//  * @param state
//  * @return New RBS::Location object.
//  * */
static VALUE rbs_location_current_token(parserstate *state) {
  return rbs_location_pp(
    state->buffer,
    &state->current_token.range.start,
    &state->current_token.range.end
  );
}

static rbs_node_t *parse_optional(parserstate *state);
static rbs_node_t *parse_simple(parserstate *state);

static VALUE string_of_loc(parserstate *state, position start, position end) {
  return rb_enc_str_new(
    RSTRING_PTR(state->lexstate->string) + start.byte_pos,
      end.byte_pos - start.byte_pos,
      rb_enc_get(state->lexstate->string)
  );
}

/**
 * Raises RuntimeError with "Unexpected error " message.
 * */
static NORETURN(void) rbs_abort(void) {
  rb_raise(
    rb_eRuntimeError,
    "Unexpected error"
  );
}

NORETURN(void) raise_syntax_error(parserstate *state, token tok, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  VALUE message = rb_vsprintf(fmt, args);
  va_end(args);

  VALUE location = rbs_new_location(state->buffer, tok.range);
  VALUE type = rb_str_new_cstr(token_type_str(tok.type));

  VALUE error = rb_funcall(
    RBS_ParsingError,
    rb_intern("new"),
    3,
    location,
    message,
    type
  );

  rb_exc_raise(error);
}

typedef enum {
  CLASS_NAME = 1,
  INTERFACE_NAME = 2,
  ALIAS_NAME = 4
} TypeNameKind;

void parser_advance_no_gap(parserstate *state) {
  if (state->current_token.range.end.byte_pos == state->next_token.range.start.byte_pos) {
    parser_advance(state);
  } else {
    raise_syntax_error(
      state,
      state->next_token,
      "unexpected token"
    );
  }
}

/*
  type_name ::= {`::`} (tUIDENT `::`)* <tXIDENT>
              | {(tUIDENT `::`)*} <tXIDENT>
              | {<tXIDENT>}
*/
rbs_typename_t *parse_type_name(parserstate *state, TypeNameKind kind, range *rg) {
  VALUE absolute = Qfalse;
  rbs_node_list_t *path = rbs_node_list_new();
  VALUE namespace;

  if (rg) {
    rg->start = state->current_token.range.start;
  }

  if (state->current_token.type == pCOLON2) {
    absolute = Qtrue;
    parser_advance_no_gap(state);
  }

  while (
    state->current_token.type == tUIDENT
    && state->next_token.type == pCOLON2
    && state->current_token.range.end.byte_pos == state->next_token.range.start.byte_pos
    && state->next_token.range.end.byte_pos == state->next_token2.range.start.byte_pos
  ) {
    VALUE symbol_value = ID2SYM(INTERN_TOKEN(state, state->current_token));
    rbs_ast_symbol_t *symbol = rbs_ast_symbol_new(symbol_value);
    rbs_node_list_append(path, (rbs_node_t *)symbol);

    parser_advance(state);
    parser_advance(state);
  }
  namespace = rbs_namespace(path->cached_ruby_value, absolute);

  switch (state->current_token.type) {
    case tLIDENT:
      if (kind & ALIAS_NAME) goto success;
      goto error;
    case tULIDENT:
      if (kind & INTERFACE_NAME) goto success;
      goto error;
    case tUIDENT:
      if (kind & CLASS_NAME) goto success;
      goto error;
    default:
      goto error;
  }

  success: {
    if (rg) {
      rg->end = state->current_token.range.end;
    }

    VALUE name = ID2SYM(INTERN_TOKEN(state, state->current_token));
    return rbs_typename_new(namespace, name);
  }

  error: {
    VALUE ids = rb_ary_new();
    if (kind & ALIAS_NAME) {
      rb_ary_push(ids, rb_str_new_literal("alias name"));
    }
    if (kind & INTERFACE_NAME) {
      rb_ary_push(ids, rb_str_new_literal("interface name"));
    }
    if (kind & CLASS_NAME) {
      rb_ary_push(ids, rb_str_new_literal("class/module/constant name"));
    }

    VALUE string = rb_funcall(ids, rb_intern("join"), 1, rb_str_new_cstr(", "));

    raise_syntax_error(
      state,
      state->current_token,
      "expected one of %"PRIsVALUE,
      string
    );
  }
}

/*
  type_list ::= {} type `,` ... <`,`> eol
              | {} type `,` ... `,` <type> eol
*/
static void parse_type_list(parserstate *state, enum TokenType eol, rbs_node_list_t *types) {
  while (true) {
    rbs_node_t *type = parse_type(state);
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
        raise_syntax_error(
          state,
          state->next_token,
          "comma delimited type list is expected"
        );
      }
    }
  }
}

static bool is_keyword_token(enum TokenType type) {
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
static rbs_types_function_param_t *parse_function_param(parserstate *state) {
  range type_range;

  type_range.start = state->next_token.range.start;
  rbs_node_t *type = parse_type(state);
  type_range.end = state->current_token.range.end;

  if (state->next_token.type == pCOMMA || state->next_token.type == pRPAREN) {
    range param_range = type_range;

    VALUE location = rbs_new_location(state->buffer, param_range);
    rbs_loc *loc = rbs_check_location(location);
    rbs_loc_alloc_children(loc, 1);
    rbs_loc_add_optional_child(loc, rb_intern("name"), NULL_RANGE);

    return rbs_types_function_param_new(type, Qnil, location);
  } else {
    range name_range = state->next_token.range;
    range param_range;

    parser_advance(state);
    param_range.start = type_range.start;
    param_range.end = name_range.end;

    if (!is_keyword_token(state->current_token.type)) {
      raise_syntax_error(
        state,
        state->current_token,
        "unexpected token for function parameter name"
      );
    }

    VALUE name = rb_to_symbol(rbs_unquote_string(state, state->current_token.range, 0));
    VALUE location = rbs_new_location(state->buffer, param_range);
    rbs_loc *loc = rbs_check_location(location);
    rbs_loc_alloc_children(loc, 1);
    rbs_loc_add_optional_child(loc, rb_intern("name"), name_range);

    return rbs_types_function_param_new(type, name, location);
  }
}

static ID intern_token_start_end(parserstate *state, token start_token, token end_token) {
  return rb_intern3(
    peek_token(state->lexstate, start_token),
    end_token.range.end.byte_pos - start_token.range.start.byte_pos,
    rb_enc_get(state->lexstate->string)
  );
}

/*
  keyword_key ::= {} <keyword> `:`
                | {} keyword <`?`> `:`
*/
static rbs_ast_symbol_t *parse_keyword_key(parserstate *state) {
  rbs_ast_symbol_t *key;

  parser_advance(state);

  if (state->next_token.type == pQUESTION) {
    key = rbs_ast_symbol_new(ID2SYM(intern_token_start_end(state, state->current_token, state->next_token)));
    parser_advance(state);
  } else {
    key = rbs_ast_symbol_new(ID2SYM(INTERN_TOKEN(state, state->current_token)));
  }

  return key;
}

/*
  keyword ::= {} keyword `:` <function_param>
*/
static void parse_keyword(parserstate *state, VALUE keywords, VALUE memo) {
  rbs_ast_symbol_t *key = parse_keyword_key(state);

  if (!NIL_P(rb_hash_aref(memo, ((rbs_node_t *)key)->cached_ruby_value))) {
    raise_syntax_error(
      state,
      state->current_token,
      "duplicated keyword argument"
    );
  } else {
    rb_hash_aset(memo, ((rbs_node_t *)key)->cached_ruby_value, Qtrue);
  }

  parser_advance_assert(state, pCOLON);
  rbs_types_function_param_t *param = parse_function_param(state);

  rb_hash_aset(keywords, ((rbs_node_t *)key)->cached_ruby_value, ((rbs_node_t *)param)->cached_ruby_value);

  return;
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
static void parse_params(parserstate *state, method_params *params) {
  if (state->next_token.type == pQUESTION && state->next_token2.type == pRPAREN) {
    params->required_positionals = NULL;
    parser_advance(state);
    return;
  }
  if (state->next_token.type == pRPAREN) {
    return;
  }

  VALUE memo = rb_hash_new();

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

        rbs_types_function_param_t *param = parse_function_param(state);
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
          parse_keyword(state, params->optional_keywords, memo);
          parser_advance_if(state, pCOMMA);
          goto PARSE_KEYWORDS;
        }

        rbs_types_function_param_t *param = parse_function_param(state);
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
    rbs_types_function_param_t *param = parse_function_param(state);
    params->rest_positionals = ((rbs_node_t *)param)->cached_ruby_value;

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

        rbs_types_function_param_t *param = parse_function_param(state);
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
        parse_keyword(state, params->optional_keywords, memo);
      } else {
        raise_syntax_error(
          state,
          state->next_token,
          "optional keyword argument type is expected"
        );
      }
      break;

    case pSTAR2:
      parser_advance(state);
      rbs_types_function_param_t *param = parse_function_param(state);
      params->rest_keywords = ((rbs_node_t *)param)->cached_ruby_value;
      break;

    case tUIDENT:
    case tLIDENT:
    case tQIDENT:
    case tULIDENT:
    case tULLIDENT:
    case tBANGIDENT:
    KEYWORD_CASES
      if (is_keyword(state)) {
        parse_keyword(state, params->required_keywords, memo);
      } else {
        raise_syntax_error(
          state,
          state->next_token,
          "required keyword argument type is expected"
        );
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
    raise_syntax_error(
      state,
      state->next_token,
      "unexpected token for method type parameters"
    );
  }

  return;
}

/*
  optional ::= {} <simple_type>
             | {} simple_type <`?`>
*/
static rbs_node_t *parse_optional(parserstate *state) {
  range rg;
  rg.start = state->next_token.range.start;
  rbs_node_t *type = parse_simple(state);

  if (state->next_token.type == pQUESTION) {
    parser_advance(state);
    rg.end = state->current_token.range.end;
    VALUE location = rbs_new_location(state->buffer, rg);
    return (rbs_node_t *) rbs_types_optional_new(type, location);
  } else {
    return type;
  }
}

static void initialize_method_params(method_params *params){
  params->required_positionals = rbs_node_list_new();
  params->optional_positionals = rbs_node_list_new();
  params->rest_positionals = Qnil;
  params->trailing_positionals = rbs_node_list_new();
  params->required_keywords = rb_hash_new();
  params->optional_keywords = rb_hash_new();
  params->rest_keywords = Qnil;
}

/*
  self_type_binding ::= {} <>
                      | {} `[` `self` `:` type <`]`>
*/
static rbs_node_t *parse_self_type_binding(parserstate *state) {
  if (state->next_token.type == pLBRACKET) {
    parser_advance(state);
    parser_advance_assert(state, kSELF);
    parser_advance_assert(state, pCOLON);
    rbs_node_t *type = parse_type(state);
    parser_advance_assert(state, pRBRACKET);
    return type;
  } else {
    return NULL;
  }
}

/*
  function ::= {} `(` params `)` self_type_binding? `{` `(` params `)` self_type_binding? `->` optional `}` `->` <optional>
             | {} `(` params `)` self_type_binding? `->` <optional>
             | {} self_type_binding? `{` `(` params `)` self_type_binding? `->` optional `}` `->` <optional>
             | {} self_type_binding? `{` self_type_binding `->` optional `}` `->` <optional>
             | {} self_type_binding? `->` <optional>
*/
static void parse_function(parserstate *state, VALUE *function, VALUE *block, VALUE *function_self_type) {
  method_params params;
  initialize_method_params(&params);

  if (state->next_token.type == pLPAREN) {
    parser_advance(state);
    parse_params(state, &params);
    parser_advance_assert(state, pRPAREN);
  }

  // Untyped method parameter means it cannot have block
  if (rbs_is_untyped_params(&params)) {
    if (state->next_token.type != pARROW) {
      raise_syntax_error(state, state->next_token2, "A method type with untyped method parameter cannot have block");
    }
  }

  // Passing NULL to function_self_type means the function itself doesn't accept self type binding. (== method type)
  if (function_self_type) {
    rbs_node_t *self_type = parse_self_type_binding(state);
    if (self_type == NULL) {
      *function_self_type = Qnil;
    } else {
      *function_self_type = self_type->cached_ruby_value;
    }
  }

  VALUE required = Qtrue;
  if (state->next_token.type == pQUESTION && state->next_token2.type == pLBRACE) {
    // Optional block
    required = Qfalse;
    parser_advance(state);
  }
  if (state->next_token.type == pLBRACE) {
    parser_advance(state);

    method_params block_params;
    initialize_method_params(&block_params);

    if (state->next_token.type == pLPAREN) {
      parser_advance(state);
      parse_params(state, &block_params);
      parser_advance_assert(state, pRPAREN);
    }

    rbs_node_t *self_type = parse_self_type_binding(state);
    VALUE block_self_type;
    if (self_type == NULL) {
      block_self_type = Qnil;
    } else {
      block_self_type = self_type->cached_ruby_value;
    }

    parser_advance_assert(state, pARROW);
    rbs_node_t *block_return_type = parse_optional(state);

    VALUE block_function = Qnil;
    if (rbs_is_untyped_params(&block_params)) {
      block_function = rbs_untyped_function(block_return_type->cached_ruby_value);
    } else {
      block_function = rbs_function(
        block_params.required_positionals->cached_ruby_value,
        block_params.optional_positionals->cached_ruby_value,
        block_params.rest_positionals,
        block_params.trailing_positionals->cached_ruby_value,
        block_params.required_keywords,
        block_params.optional_keywords,
        block_params.rest_keywords,
        block_return_type->cached_ruby_value
      );
    }

    *block = rbs_block(block_function, required, block_self_type);

    parser_advance_assert(state, pRBRACE);
  }

  parser_advance_assert(state, pARROW);
  rbs_node_t *type = parse_optional(state);

  if (rbs_is_untyped_params(&params)) {
    *function = rbs_untyped_function(type->cached_ruby_value);
  } else {
    *function = rbs_function(
      params.required_positionals->cached_ruby_value,
      params.optional_positionals->cached_ruby_value,
      params.rest_positionals,
      params.trailing_positionals->cached_ruby_value,
      params.required_keywords,
      params.optional_keywords,
      params.rest_keywords,
      type->cached_ruby_value
    );
  }
}

/*
  proc_type ::= {`^`} <function>
*/
static rbs_types_proc_t *parse_proc_type(parserstate *state) {
  position start = state->current_token.range.start;
  VALUE function = Qnil;
  VALUE block = Qnil;
  VALUE proc_self = Qnil;
  parse_function(state, &function, &block, &proc_self);
  position end = state->current_token.range.end;
  VALUE loc = rbs_location_pp(state->buffer, &start, &end);
  return rbs_types_proc_new(function, block, loc, proc_self);
}

static void check_key_duplication(parserstate *state, VALUE fields, VALUE key) {
  if (!NIL_P(rb_hash_aref(fields, key))) {
    raise_syntax_error(
      state,
      state->current_token,
      "duplicated record key"
    );
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
VALUE parse_record_attributes(parserstate *state) {
  VALUE fields = rb_hash_new();

  if (state->next_token.type == pRBRACE) {
    return fields;
  }

  while (true) {
    rbs_ast_symbol_t *key;
    VALUE value = rb_ary_new();
    VALUE required = Qtrue;

    if (state->next_token.type == pQUESTION) {
      // { ?foo: type } syntax
      required = Qfalse;
      parser_advance(state);
    }

    if (is_keyword(state)) {
      // { foo: type } syntax
      key = parse_keyword_key(state);
      check_key_duplication(state, fields, ((rbs_node_t *)key)->cached_ruby_value);
      parser_advance_assert(state, pCOLON);
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
      case kFALSE:
        key = rbs_ast_symbol_new(rb_funcall(rbs_struct_to_ruby_value(parse_simple(state)), rb_intern("literal"), 0));
        break;
      default:
        raise_syntax_error(
          state,
          state->next_token,
          "unexpected record key token"
        );
      }
      check_key_duplication(state, fields, ((rbs_node_t *)key)->cached_ruby_value);
      parser_advance_assert(state, pFATARROW);
    }
    rbs_node_t *type = parse_type(state);
    rb_ary_push(value, type->cached_ruby_value);
    rb_ary_push(value, required);
    rb_hash_aset(fields, ((rbs_node_t *)key)->cached_ruby_value, value);

    if (parser_advance_if(state, pCOMMA)) {
      if (state->next_token.type == pRBRACE) {
        break;
      }
    } else {
      break;
    }
  }
  return fields;
}

/*
  symbol ::= {<tSYMBOL>}
*/
static rbs_types_literal_t *parse_symbol(parserstate *state, VALUE location) {
  VALUE string = state->lexstate->string;
  rb_encoding *enc = rb_enc_get(string);

  int offset_bytes = rb_enc_codelen(':', enc);
  int bytes = token_bytes(state->current_token) - offset_bytes;

  rbs_ast_symbol_t *literal;

  switch (state->current_token.type)
  {
  case tSYMBOL: {
    char *buffer = peek_token(state->lexstate, state->current_token);
    literal = rbs_ast_symbol_new(ID2SYM(rb_intern3(buffer+offset_bytes, bytes, enc)));
    break;
  }
  case tDQSYMBOL:
  case tSQSYMBOL: {
    literal = rbs_ast_symbol_new(
      rb_funcall(
        rbs_unquote_string(state, state->current_token.range, offset_bytes),
        rb_intern("to_sym"),
        0
      )
    );
    break;
  }
  default:
    rbs_abort();
  }

  return rbs_types_literal_new(((rbs_node_t *)literal)->cached_ruby_value, location);
}

/*
 instance_type ::= {type_name} <type_args>

 type_args ::= {} <> /empty/
             | {} `[` type_list <`]`>
 */
static rbs_node_t *parse_instance_type(parserstate *state, bool parse_alias) {
    range name_range;
    range args_range;
    range type_range;

    TypeNameKind expected_kind = INTERFACE_NAME | CLASS_NAME;
    if (parse_alias) {
      expected_kind |= ALIAS_NAME;
    }

    rbs_typename_t *typename = parse_type_name(state, expected_kind, &name_range);
    rbs_node_list_t *types = rbs_node_list_new();

    TypeNameKind kind;
    if (state->current_token.type == tUIDENT) {
      kind = CLASS_NAME;
    } else if (state->current_token.type == tULIDENT) {
      kind = INTERFACE_NAME;
    } else if (state->current_token.type == tLIDENT) {
      kind = ALIAS_NAME;
    } else {
      rbs_abort();
    }

    if (state->next_token.type == pLBRACKET) {
      parser_advance(state);
      args_range.start = state->current_token.range.start;
      parse_type_list(state, pRBRACKET, types);
      parser_advance_assert(state, pRBRACKET);
      args_range.end = state->current_token.range.end;
    } else {
      args_range = NULL_RANGE;
    }

    type_range.start = name_range.start;
    type_range.end = nonnull_pos_or(args_range.end, name_range.end);

    VALUE location = rbs_new_location(state->buffer, type_range);
    rbs_loc *loc = rbs_check_location(location);
    rbs_loc_alloc_children(loc, 2);
    rbs_loc_add_required_child(loc, rb_intern("name"), name_range);
    rbs_loc_add_optional_child(loc, rb_intern("args"), args_range);

    if (kind == CLASS_NAME) {
      return (rbs_node_t *) rbs_types_classinstance_new(typename, types->cached_ruby_value, location);
    } else if (kind == INTERFACE_NAME) {
      return (rbs_node_t *) rbs_types_interface_new(typename, types->cached_ruby_value, location);
    } else if (kind == ALIAS_NAME) {
      return (rbs_node_t *) rbs_types_alias_new(typename, types->cached_ruby_value, location);
    } else {
      return NULL;
    }
}

/*
  singleton_type ::= {`singleton`} `(` type_name <`)`>
*/
static rbs_types_classsingleton_t *parse_singleton_type(parserstate *state) {
  range name_range;
  range type_range;

  parser_assert(state, kSINGLETON);

  type_range.start = state->current_token.range.start;
  parser_advance_assert(state, pLPAREN);
  parser_advance(state);

  rbs_typename_t *typename = parse_type_name(state, CLASS_NAME, &name_range);

  parser_advance_assert(state, pRPAREN);
  type_range.end = state->current_token.range.end;

  VALUE location = rbs_new_location(state->buffer, type_range);
  rbs_loc *loc = rbs_check_location(location);
  rbs_loc_alloc_children(loc, 1);
  rbs_loc_add_required_child(loc, rb_intern("name"), name_range);

  return rbs_types_classsingleton_new(typename, location);
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
static rbs_node_t *parse_simple(parserstate *state) {
  parser_advance(state);

  switch (state->current_token.type) {
  case pLPAREN: {
    rbs_node_t *type = parse_type(state);
    parser_advance_assert(state, pRPAREN);
    return type;
  }
  case kBOOL: {
    VALUE loc = rbs_location_current_token(state);
    return (rbs_node_t *) rbs_types_bases_bool_new(loc);
  }
  case kBOT: {
    VALUE loc = rbs_location_current_token(state);
    return (rbs_node_t *) rbs_types_bases_bottom_new(loc);
  }
  case kCLASS: {
    VALUE loc = rbs_location_current_token(state);
    return (rbs_node_t *) rbs_types_bases_class_new(loc);
  }
  case kINSTANCE: {
    VALUE loc = rbs_location_current_token(state);
    return (rbs_node_t *) rbs_types_bases_instance_new(loc);
  }
  case kNIL: {
    VALUE loc = rbs_location_current_token(state);
    return (rbs_node_t *) rbs_types_bases_nil_new(loc);
  }
  case kSELF: {
    VALUE loc = rbs_location_current_token(state);
    return (rbs_node_t *) rbs_types_bases_self_new(loc);
  }
  case kTOP: {
    VALUE loc = rbs_location_current_token(state);
    return (rbs_node_t *) rbs_types_bases_top_new(loc);
  }
  case kVOID: {
    VALUE loc = rbs_location_current_token(state);
    return (rbs_node_t *) rbs_types_bases_void_new(loc);
  }
  case kUNTYPED: {
    VALUE loc = rbs_location_current_token(state);
    return (rbs_node_t *) rbs_types_bases_any_new(loc);
  }
  case k__TODO__: {
    VALUE loc = rbs_location_current_token(state);
    rbs_types_bases_any_t *node = rbs_types_bases_any_new(loc);
    rb_funcall(node->base.cached_ruby_value, rb_intern("todo!"), 0);
    return (rbs_node_t *) node;
  }
  case tINTEGER: {
    VALUE loc = rbs_location_current_token(state);
    VALUE literal = rb_funcall(
      string_of_loc(state, state->current_token.range.start, state->current_token.range.end),
      rb_intern("to_i"),
      0
    );
    return (rbs_node_t *) rbs_types_literal_new(literal, loc);
  }
  case kTRUE: {
    VALUE loc = rbs_location_current_token(state);
    return (rbs_node_t *) rbs_types_literal_new(Qtrue, loc);
  }
  case kFALSE: {
    VALUE loc = rbs_location_current_token(state);
    return (rbs_node_t *) rbs_types_literal_new(Qfalse, loc);
  }
  case tSQSTRING:
  case tDQSTRING: {
    VALUE loc = rbs_location_current_token(state);
    VALUE literal = rbs_unquote_string(state, state->current_token.range, 0);
    return (rbs_node_t *) rbs_types_literal_new(literal, loc);
  }
  case tSYMBOL:
  case tSQSYMBOL:
  case tDQSYMBOL: {
    VALUE loc = rbs_location_current_token(state);
    return (rbs_node_t *) parse_symbol(state, loc);
  }
  case tUIDENT: {
    ID name = INTERN_TOKEN(state, state->current_token);
    if (parser_typevar_member(state, name)) {
      VALUE loc = rbs_location_current_token(state);
      return (rbs_node_t *) rbs_types_variable_new(ID2SYM(name), loc);
    }
    // fallthrough for type name
  }
  case tULIDENT: // fallthrough
  case tLIDENT: // fallthrough
  case pCOLON2: {
    return parse_instance_type(state, true);
  }
  case kSINGLETON: {
    return (rbs_node_t *) parse_singleton_type(state);
  }
  case pLBRACKET: {
    range rg;
    rg.start = state->current_token.range.start;
    rbs_node_list_t *types = rbs_node_list_new();
    if (state->next_token.type != pRBRACKET) {
      parse_type_list(state, pRBRACKET, types);
    }
    parser_advance_assert(state, pRBRACKET);
    rg.end = state->current_token.range.end;

    VALUE location = rbs_new_location(state->buffer, rg);
    return (rbs_node_t *) rbs_types_tuple_new(types->cached_ruby_value, location);
  }
  case pAREF_OPR: {
    VALUE loc = rbs_location_current_token(state);
    rbs_node_list_t *types = rbs_node_list_new();
    return (rbs_node_t *) rbs_types_tuple_new(types->cached_ruby_value, loc);
  }
  case pLBRACE: {
    position start = state->current_token.range.start;
    VALUE fields = parse_record_attributes(state);
    parser_advance_assert(state, pRBRACE);
    position end = state->current_token.range.end;
    VALUE loc = rbs_location_pp(state->buffer, &start, &end);
    return (rbs_node_t *) rbs_types_record_new(fields, loc);
  }
  case pHAT: {
    rbs_types_proc_t *value = parse_proc_type(state);
    return (rbs_node_t *) value;
  }
  default:
    raise_syntax_error(
      state,
      state->current_token,
      "unexpected token for simple type"
    );
  }
}

/*
  intersection ::= {} optional `&` ... '&' <optional>
                 | {} <optional>
*/
static rbs_node_t *parse_intersection(parserstate *state) {
  range rg;

  rg.start = state->next_token.range.start;
  rbs_node_t *type = parse_optional(state);
  VALUE intersection_types = rb_ary_new();

  rb_ary_push(intersection_types, type->cached_ruby_value);
  while (state->next_token.type == pAMP) {
    parser_advance(state);
    rb_ary_push(intersection_types, parse_optional(state)->cached_ruby_value);
  }

  rg.end = state->current_token.range.end;

  if (rb_array_len(intersection_types) > 1) {
    VALUE location = rbs_new_location(state->buffer, rg);
    type = (rbs_node_t *) rbs_types_intersection_new(intersection_types, location);
  }

  return type;
}

/*
  union ::= {} intersection '|' ... '|' <intersection>
          | {} <intersection>
*/
rbs_node_t *parse_type(parserstate *state) {
  range rg;

  rg.start = state->next_token.range.start;
  rbs_node_t *type = parse_intersection(state);
  VALUE union_types = rb_ary_new();

  rb_ary_push(union_types, type->cached_ruby_value);
  while (state->next_token.type == pBAR) {
    parser_advance(state);
    rb_ary_push(union_types, parse_intersection(state)->cached_ruby_value);
  }

  rg.end = state->current_token.range.end;

  if (rb_array_len(union_types) > 1) {
    VALUE location = rbs_new_location(state->buffer, rg);
    type = (rbs_node_t *) rbs_types_union_new(union_types, location);
  }

  return type;
}

/*
  type_params ::= {} `[` type_param `,` ... <`]`>
                | {<>}

  type_param ::= kUNCHECKED? (kIN|kOUT|) tUIDENT upper_bound? default_type?   (module_type_params == true)

  type_param ::= tUIDENT upper_bound? default_type?                           (module_type_params == false)
*/
rbs_node_list_t *parse_type_params(parserstate *state, range *rg, bool module_type_params) {
  rbs_node_list_t *params = rbs_node_list_new();

  bool required_param_allowed = true;

  if (state->next_token.type == pLBRACKET) {
    parser_advance(state);

    rg->start = state->current_token.range.start;

    while (true) {
      rbs_ast_symbol_t *name;
      bool unchecked = false;
      rbs_ast_symbol_t *variance = rbs_ast_symbol_new(ID2SYM(rb_intern("invariant")));
      VALUE upper_bound = Qnil;
      VALUE default_type = Qnil;

      range param_range = NULL_RANGE;
      range name_range;
      range variance_range = NULL_RANGE;
      range unchecked_range = NULL_RANGE;
      range upper_bound_range = NULL_RANGE;
      range default_type_range = NULL_RANGE;

      param_range.start = state->next_token.range.start;

      if (module_type_params) {
        if (state->next_token.type == kUNCHECKED) {
          unchecked = true;
          parser_advance(state);
          unchecked_range = state->current_token.range;
        }

        if (state->next_token.type == kIN || state->next_token.type == kOUT) {
          switch (state->next_token.type) {
          case kIN:
            variance = rbs_ast_symbol_new(ID2SYM(rb_intern("contravariant")));
            break;
          case kOUT:
            variance = rbs_ast_symbol_new(ID2SYM(rb_intern("covariant")));
            break;
          default:
            rbs_abort();
          }

          parser_advance(state);
          variance_range = state->current_token.range;
        }
      }

      parser_advance_assert(state, tUIDENT);
      name_range = state->current_token.range;

      ID id = INTERN_TOKEN(state, state->current_token);
      name = rbs_ast_symbol_new(ID2SYM(id));

      parser_insert_typevar(state, id);

      if (state->next_token.type == pLT) {
        parser_advance(state);
        upper_bound_range.start = state->current_token.range.start;
        upper_bound = parse_type(state)->cached_ruby_value;
        upper_bound_range.end = state->current_token.range.end;
      }

      if (module_type_params) {
        if (state->next_token.type == pEQ) {
          parser_advance(state);

          default_type_range.start = state->current_token.range.start;
          default_type = parse_type(state)->cached_ruby_value;
          default_type_range.end = state->current_token.range.end;

          required_param_allowed = false;
        } else {
          if (!required_param_allowed) {
            raise_syntax_error(
              state,
              state->current_token,
              "required type parameter is not allowed after optional type parameter"
            );
          }
        }
      }

      param_range.end = state->current_token.range.end;

      VALUE location = rbs_new_location(state->buffer, param_range);
      rbs_loc *loc = rbs_check_location(location);
      rbs_loc_alloc_children(loc, 4);
      rbs_loc_add_required_child(loc, rb_intern("name"), name_range);
      rbs_loc_add_optional_child(loc, rb_intern("variance"), variance_range);
      rbs_loc_add_optional_child(loc, rb_intern("unchecked"), unchecked_range);
      rbs_loc_add_optional_child(loc, rb_intern("upper_bound"), upper_bound_range);
      rbs_loc_add_optional_child(loc, rb_intern("default"), default_type_range);

      rbs_ast_typeparam_t *param = rbs_ast_typeparam_new(((rbs_node_t *)name)->cached_ruby_value, ((rbs_node_t *)variance)->cached_ruby_value, upper_bound, default_type, location);

      if (unchecked) {
        rb_funcall(((rbs_node_t *) param)->cached_ruby_value, rb_intern("unchecked!"), 0);
      }

      rbs_node_list_append(params, (rbs_node_t *) param);

      if (state->next_token.type == pCOMMA) {
        parser_advance(state);
      }

      if (state->next_token.type == pRBRACKET) {
        break;
      }
    }

    parser_advance_assert(state, pRBRACKET);
    rg->end = state->current_token.range.end;
  } else {
    *rg = NULL_RANGE;
  }

  rb_funcall(
    RBS_AST_TypeParam,
    rb_intern("resolve_variables"),
    1,
    params->cached_ruby_value
  );

  return params;
}

/*
  method_type ::= {} type_params <function>
  */
rbs_methodtype_t *parse_method_type(parserstate *state) {
  range rg;
  range params_range = NULL_RANGE;
  range type_range;

  VALUE function = Qnil;
  VALUE block = Qnil;
  parser_push_typevar_table(state, false);

  rg.start = state->next_token.range.start;

  rbs_node_list_t *type_params = parse_type_params(state, &params_range, false);

  type_range.start = state->next_token.range.start;

  parse_function(state, &function, &block, NULL);

  rg.end = state->current_token.range.end;
  type_range.end = rg.end;

  parser_pop_typevar_table(state);

  VALUE location = rbs_new_location(state->buffer, rg);
  rbs_loc *loc = rbs_check_location(location);
  rbs_loc_alloc_children(loc, 2);
  rbs_loc_add_required_child(loc, rb_intern("type"), type_range);
  rbs_loc_add_optional_child(loc, rb_intern("type_params"), params_range);

  return rbs_methodtype_new(type_params, function, block, location);
}

/*
  global_decl ::= {tGIDENT} `:` <type>
*/
rbs_ast_declarations_global_t *parse_global_decl(parserstate *state) {
  range decl_range;
  range name_range, colon_range;

  rbs_ast_symbol_t *typename;
  rbs_node_t *type;
  VALUE location;
  VALUE comment;

  rbs_loc *loc;

  decl_range.start = state->current_token.range.start;
  comment = get_comment(state, decl_range.start.line);

  name_range = state->current_token.range;
  typename = rbs_ast_symbol_new(ID2SYM(INTERN_TOKEN(state, state->current_token)));

  parser_advance_assert(state, pCOLON);
  colon_range = state->current_token.range;

  type = parse_type(state);
  decl_range.end = state->current_token.range.end;

  location = rbs_new_location(state->buffer, decl_range);
  loc = rbs_check_location(location);
  rbs_loc_alloc_children(loc, 2);
  rbs_loc_add_required_child(loc, rb_intern("name"), name_range);
  rbs_loc_add_required_child(loc, rb_intern("colon"), colon_range);

  return rbs_ast_declarations_global_new(((rbs_node_t *)typename)->cached_ruby_value, type, location, comment);
}

/*
  const_decl ::= {const_name} `:` <type>
*/
rbs_ast_declarations_constant_t *parse_const_decl(parserstate *state) {
  range decl_range;
  range name_range, colon_range;

  rbs_typename_t *typename;
  rbs_node_t *type;
  VALUE location;
  VALUE comment;

  rbs_loc *loc;

  decl_range.start = state->current_token.range.start;
  comment = get_comment(state, decl_range.start.line);

  typename = parse_type_name(state, CLASS_NAME, &name_range);

  parser_advance_assert(state, pCOLON);
  colon_range = state->current_token.range;

  type = parse_type(state);
  decl_range.end = state->current_token.range.end;

  location = rbs_new_location(state->buffer, decl_range);
  loc = rbs_check_location(location);
  rbs_loc_alloc_children(loc, 2);
  rbs_loc_add_required_child(loc, rb_intern("name"), name_range);
  rbs_loc_add_required_child(loc, rb_intern("colon"), colon_range);

  return rbs_ast_declarations_constant_new(typename, type, location, comment);
}

/*
  type_decl ::= {kTYPE} alias_name `=` <type>
*/
rbs_ast_declarations_typealias_t *parse_type_decl(parserstate *state, position comment_pos, rbs_node_list_t *annotations) {
  range decl_range;
  range keyword_range, name_range, params_range, eq_range;

  parser_push_typevar_table(state, true);

  decl_range.start = state->current_token.range.start;
  comment_pos = nonnull_pos_or(comment_pos, decl_range.start);

  keyword_range = state->current_token.range;

  parser_advance(state);
  rbs_typename_t *typename = parse_type_name(state, ALIAS_NAME, &name_range);

  rbs_node_list_t *type_params = parse_type_params(state, &params_range, true);

  parser_advance_assert(state, pEQ);
  eq_range = state->current_token.range;

  rbs_node_t *type = parse_type(state);
  decl_range.end = state->current_token.range.end;

  VALUE location = rbs_new_location(state->buffer, decl_range);
  rbs_loc *loc = rbs_check_location(location);
  rbs_loc_alloc_children(loc, 4);
  rbs_loc_add_required_child(loc, rb_intern("keyword"), keyword_range);
  rbs_loc_add_required_child(loc, rb_intern("name"), name_range);
  rbs_loc_add_optional_child(loc, rb_intern("type_params"), params_range);
  rbs_loc_add_required_child(loc, rb_intern("eq"), eq_range);

  parser_pop_typevar_table(state);

  VALUE comment = get_comment(state, comment_pos.line);
  return rbs_ast_declarations_typealias_new(typename, type_params, type, annotations, location, comment);
}

/*
  annotation ::= {<tANNOTATION>}
*/
rbs_ast_annotation_t *parse_annotation(parserstate *state) {
  VALUE content = rb_funcall(state->buffer, rb_intern("content"), 0);
  rb_encoding *enc = rb_enc_get(content);

  range rg = state->current_token.range;

  int offset_bytes = rb_enc_codelen('%', enc) + rb_enc_codelen('a', enc);

  unsigned int open_char = rb_enc_mbc_to_codepoint(
    RSTRING_PTR(state->lexstate->string) + rg.start.byte_pos + offset_bytes,
    RSTRING_END(state->lexstate->string),
    enc
  );

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
    rbs_abort();
  }

  int open_bytes = rb_enc_codelen(open_char, enc);
  int close_bytes = rb_enc_codelen(close_char, enc);

  char *buffer = RSTRING_PTR(state->lexstate->string) + rg.start.byte_pos + offset_bytes + open_bytes;
  VALUE string = rb_enc_str_new(
    buffer,
    rg.end.byte_pos - rg.start.byte_pos - offset_bytes - open_bytes - close_bytes,
    enc
  );
  rb_funcall(string, rb_intern("strip!"), 0);

  VALUE location = rbs_location_current_token(state);
  return rbs_ast_annotation_new(string, location);
}

/*
  annotations ::= {} annotation ... <annotation>
                | {<>}
*/
void parse_annotations(parserstate *state, rbs_node_list_t *annotations, position *annot_pos) {
  *annot_pos = NullPosition;

  while (true) {
    if (state->next_token.type == tANNOTATION) {
      parser_advance(state);

      if (null_position_p((*annot_pos))) {
        *annot_pos = state->current_token.range.start;
      }

      rbs_node_list_append(annotations, (rbs_node_t *)parse_annotation(state));
    } else {
      break;
    }
  }
}

/*
  method_name ::= {} <IDENT | keyword>
                | {} (IDENT | keyword)~<`?`>
*/
rbs_ast_symbol_t *parse_method_name(parserstate *state, range *range) {
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

      ID id = rb_intern3(
        RSTRING_PTR(state->lexstate->string) + range->start.byte_pos,
        range->end.byte_pos - range->start.byte_pos,
        rb_enc_get(state->lexstate->string)
      );

      return rbs_ast_symbol_new(ID2SYM(id));
    } else {
      *range = state->current_token.range;
      return rbs_ast_symbol_new(ID2SYM(INTERN_TOKEN(state, state->current_token)));
    }

  case tBANGIDENT:
  case tEQIDENT:
    *range = state->current_token.range;
    return rbs_ast_symbol_new(ID2SYM(INTERN_TOKEN(state, state->current_token)));

  case tQIDENT:
    return rbs_ast_symbol_new(rb_to_symbol(rbs_unquote_string(state, state->current_token.range, 0)));

  case pBAR:
  case pHAT:
  case pAMP:
  case pSTAR:
  case pSTAR2:
  case pLT:
  case pAREF_OPR:
  case tOPERATOR:
    *range = state->current_token.range;
    return rbs_ast_symbol_new(ID2SYM(INTERN_TOKEN(state, state->current_token)));

  default:
    raise_syntax_error(
      state,
      state->current_token,
      "unexpected token for method name"
    );
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
InstanceSingletonKind parse_instance_singleton_kind(parserstate *state, bool allow_selfq, range *rg) {
  InstanceSingletonKind kind = INSTANCE_KIND;

  if (state->next_token.type == kSELF) {
    range self_range = state->next_token.range;

    if (state->next_token2.type == pDOT) {
      parser_advance(state);
      parser_advance(state);
      kind = SINGLETON_KIND;
      rg->start = self_range.start;
      rg->end = state->current_token.range.end;
    } else if (
      state->next_token2.type == pQUESTION
    && state->next_token.range.end.char_pos == state->next_token2.range.start.char_pos
    && state->next_token3.type == pDOT
    && allow_selfq) {
      parser_advance(state);
      parser_advance(state);
      parser_advance(state);
      kind = INSTANCE_SINGLETON_KIND;
      rg->start = self_range.start;
      rg->end = state->current_token.range.end;
    }
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
rbs_ast_members_methoddefinition_t *parse_member_def(parserstate *state, bool instance_only, bool accept_overload, position comment_pos, rbs_node_list_t *annotations) {
  range member_range;
  range visibility_range;
  range keyword_range;
  range name_range;
  range kind_range;
  range overloading_range = NULL_RANGE;

  VALUE visibility;

  member_range.start = state->current_token.range.start;
  comment_pos = nonnull_pos_or(comment_pos, member_range.start);
  VALUE comment = get_comment(state, comment_pos.line);

  switch (state->current_token.type)
  {
  case kPRIVATE:
    visibility_range = state->current_token.range;
    visibility = ID2SYM(rb_intern("private"));
    member_range.start = visibility_range.start;
    parser_advance(state);
    break;
  case kPUBLIC:
    visibility_range = state->current_token.range;
    visibility = ID2SYM(rb_intern("public"));
    member_range.start = visibility_range.start;
    parser_advance(state);
    break;
  default:
    visibility_range = NULL_RANGE;
    visibility = Qnil;
    break;
  }

  keyword_range = state->current_token.range;

  InstanceSingletonKind kind;
  if (instance_only) {
    kind_range = NULL_RANGE;
    kind = INSTANCE_KIND;
  } else {
    kind = parse_instance_singleton_kind(state, NIL_P(visibility), &kind_range);
  }

  rbs_ast_symbol_t *name = parse_method_name(state, &name_range);
  VALUE overloads = rb_ary_new();
  VALUE overloading = Qfalse;

  if (state->next_token.type == pDOT && RB_SYM2ID(((rbs_node_t *)name)->cached_ruby_value) == rb_intern("self?")) {
    raise_syntax_error(
      state,
      state->next_token,
      "`self?` method cannot have visibility"
    );
  } else {
    parser_advance_assert(state, pCOLON);
  }

  parser_push_typevar_table(state, kind != INSTANCE_KIND);

  bool loop = true;
  while (loop) {
    rbs_node_list_t *annotations = rbs_node_list_new();
    position overload_annot_pos = NullPosition;

    if (state->next_token.type == tANNOTATION) {
      parse_annotations(state, annotations, &overload_annot_pos);
    }

    switch (state->next_token.type) {
    case pLPAREN:
    case pARROW:
    case pLBRACE:
    case pLBRACKET:
    case pQUESTION:
      {
        rbs_methodtype_t *method_type = parse_method_type(state);
        rb_ary_push(overloads, rbs_ast_members_method_definition_overload(annotations->cached_ruby_value, ((rbs_node_t *)method_type)->cached_ruby_value));
        member_range.end = state->current_token.range.end;
        break;
      }

    case pDOT3:
      if (accept_overload) {
        overloading = Qtrue;
        parser_advance(state);
        loop = false;
        overloading_range = state->current_token.range;
        member_range.end = overloading_range.end;
        break;
      } else {
        raise_syntax_error(
          state,
          state->next_token,
          "unexpected overloading method definition"
        );
      }

    default:
      raise_syntax_error(
        state,
        state->next_token,
        "unexpected token for method type"
      );
    }

    if (state->next_token.type == pBAR) {
      parser_advance(state);
    } else {
      loop = false;
    }
  }

  parser_pop_typevar_table(state);

  rbs_ast_symbol_t *k;
  switch (kind) {
  case INSTANCE_KIND:
    k = rbs_ast_symbol_new(ID2SYM(rb_intern("instance")));
    break;
  case SINGLETON_KIND:
    k = rbs_ast_symbol_new(ID2SYM(rb_intern("singleton")));
    break;
  case INSTANCE_SINGLETON_KIND:
    k = rbs_ast_symbol_new(ID2SYM(rb_intern("singleton_instance")));
    break;
  default:
    rbs_abort();
  }

  VALUE location = rbs_new_location(state->buffer, member_range);
  rbs_loc *loc = rbs_check_location(location);
  rbs_loc_alloc_children(loc, 5);
  rbs_loc_add_required_child(loc, rb_intern("keyword"), keyword_range);
  rbs_loc_add_required_child(loc, rb_intern("name"), name_range);
  rbs_loc_add_optional_child(loc, rb_intern("kind"), kind_range);
  rbs_loc_add_optional_child(loc, rb_intern("overloading"), overloading_range);
  rbs_loc_add_optional_child(loc, rb_intern("visibility"), visibility_range);

  return rbs_ast_members_methoddefinition_new(((rbs_node_t *)name)->cached_ruby_value, ((rbs_node_t *)k)->cached_ruby_value, overloads, annotations, location, comment, overloading, visibility);
}

/**
 * class_instance_name ::= {} <class_name>
 *                       | {} class_name `[` type args <`]`>
 *
 * @param kind
 * */
void class_instance_name(parserstate *state, TypeNameKind kind, VALUE *name, rbs_node_list_t *args, range *name_range, range *args_range) {
  parser_advance(state);

  *name = ((rbs_node_t *)parse_type_name(state, kind, name_range))->cached_ruby_value;

  if (state->next_token.type == pLBRACKET) {
    parser_advance(state);
    args_range->start = state->current_token.range.start;
    parse_type_list(state, pRBRACKET, args);
    parser_advance_assert(state, pRBRACKET);
    args_range->end = state->current_token.range.end;
  } else {
    *args_range = NULL_RANGE;
  }
}

/**
 *  mixin_member ::= {kINCLUDE} <class_instance_name>
 *                 | {kPREPEND} <class_instance_name>
 *                 | {kEXTEND} <class_instance_name>
 *
 * @param from_interface `true` when the member is in an interface.
 * */
rbs_node_t *parse_mixin_member(parserstate *state, bool from_interface, position comment_pos, rbs_node_list_t *annotations) {
  range member_range;
  range name_range;
  range keyword_range;
  range args_range = NULL_RANGE;
  bool reset_typevar_scope;
  enum TokenType type;

  member_range.start = state->current_token.range.start;
  comment_pos = nonnull_pos_or(comment_pos, member_range.start);

  type = state->current_token.type;
  keyword_range = state->current_token.range;

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
    rbs_abort();
  }

  if (from_interface) {
    if (state->current_token.type != kINCLUDE) {
      raise_syntax_error(
        state,
        state->current_token,
        "unexpected mixin in interface declaration"
      );
    }
  }

  parser_push_typevar_table(state, reset_typevar_scope);

  VALUE name;
  rbs_node_list_t *args = rbs_node_list_new();
  class_instance_name(
    state,
    from_interface ? INTERFACE_NAME : (INTERFACE_NAME | CLASS_NAME),
    &name, args, &name_range, &args_range
  );

  parser_pop_typevar_table(state);

  member_range.end = state->current_token.range.end;

  VALUE location = rbs_new_location(state->buffer, member_range);
  rbs_loc *loc = rbs_check_location(location);
  rbs_loc_alloc_children(loc, 3);
  rbs_loc_add_required_child(loc, rb_intern("name"), name_range);
  rbs_loc_add_required_child(loc, rb_intern("keyword"), keyword_range);
  rbs_loc_add_optional_child(loc, rb_intern("args"), args_range);

  VALUE comment = get_comment(state, comment_pos.line);
  switch (type)
  {
  case kINCLUDE:
    return (rbs_node_t *) rbs_ast_members_include_new(name, args->cached_ruby_value, annotations, location, comment);
  case kEXTEND:
    return (rbs_node_t *) rbs_ast_members_extend_new(name, args->cached_ruby_value, annotations, location, comment);
  case kPREPEND:
    return (rbs_node_t *) rbs_ast_members_prepend_new(name, args->cached_ruby_value, annotations, location, comment);
  default:
    rbs_abort();
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
rbs_ast_members_alias_t *parse_alias_member(parserstate *state, bool instance_only, position comment_pos, rbs_node_list_t *annotations) {
  range member_range;
  range keyword_range, new_name_range, old_name_range;
  range new_kind_range, old_kind_range;

  member_range.start = state->current_token.range.start;
  keyword_range = state->current_token.range;

  comment_pos = nonnull_pos_or(comment_pos, member_range.start);
  VALUE comment = get_comment(state, comment_pos.line);

  rbs_ast_symbol_t *new_name;
  rbs_ast_symbol_t *old_name;
  rbs_ast_symbol_t *kind;

  if (!instance_only && state->next_token.type == kSELF) {
    kind = rbs_ast_symbol_new(ID2SYM(rb_intern("singleton")));

    new_kind_range.start = state->next_token.range.start;
    new_kind_range.end = state->next_token2.range.end;
    parser_advance_assert(state, kSELF);
    parser_advance_assert(state, pDOT);
    new_name = parse_method_name(state, &new_name_range);

    old_kind_range.start = state->next_token.range.start;
    old_kind_range.end = state->next_token2.range.end;
    parser_advance_assert(state, kSELF);
    parser_advance_assert(state, pDOT);
    old_name = parse_method_name(state, &old_name_range);
  } else {
    kind = rbs_ast_symbol_new(ID2SYM(rb_intern("instance")));
    new_name = parse_method_name(state, &new_name_range);
    old_name = parse_method_name(state, &old_name_range);

    new_kind_range = NULL_RANGE;
    old_kind_range = NULL_RANGE;
  }

  member_range.end = state->current_token.range.end;
  VALUE location = rbs_new_location(state->buffer, member_range);
  rbs_loc *loc = rbs_check_location(location);
  rbs_loc_alloc_children(loc, 5);
  rbs_loc_add_required_child(loc, rb_intern("keyword"), keyword_range);
  rbs_loc_add_required_child(loc, rb_intern("new_name"), new_name_range);
  rbs_loc_add_required_child(loc, rb_intern("old_name"), old_name_range);
  rbs_loc_add_optional_child(loc, rb_intern("new_kind"), new_kind_range);
  rbs_loc_add_optional_child(loc, rb_intern("old_kind"), old_kind_range);

  return rbs_ast_members_alias_new(((rbs_node_t *)new_name)->cached_ruby_value, ((rbs_node_t *)old_name)->cached_ruby_value, ((rbs_node_t *)kind)->cached_ruby_value, annotations, location, comment);
}

/*
  variable_member ::= {tAIDENT} `:` <type>
                    | {kSELF} `.` tAIDENT `:` <type>
                    | {tA2IDENT} `:` <type>
*/
rbs_node_t *parse_variable_member(parserstate *state, position comment_pos, rbs_node_list_t *annotations) {
  range member_range;
  range name_range, colon_range;
  range kind_range = NULL_RANGE;
  rbs_loc *loc;

  if (annotations->length > 0) {
    raise_syntax_error(
      state,
      state->current_token,
      "annotation cannot be given to variable members"
    );
  }

  member_range.start = state->current_token.range.start;
  comment_pos = nonnull_pos_or(comment_pos, member_range.start);
  VALUE comment = get_comment(state, comment_pos.line);

  VALUE location;
  rbs_ast_symbol_t *name;
  rbs_node_t *type;

  switch (state->current_token.type)
  {
  case tAIDENT:
    name_range = state->current_token.range;
    name = rbs_ast_symbol_new(ID2SYM(INTERN_TOKEN(state, state->current_token)));

    parser_advance_assert(state, pCOLON);
    colon_range = state->current_token.range;

    type = parse_type(state);
    member_range.end = state->current_token.range.end;

    location = rbs_new_location(state->buffer, member_range);
    loc = rbs_check_location(location);
    rbs_loc_alloc_children(loc, 3);
    rbs_loc_add_required_child(loc, rb_intern("name"), name_range);
    rbs_loc_add_required_child(loc, rb_intern("colon"), colon_range);
    rbs_loc_add_optional_child(loc, rb_intern("kind"), kind_range);

    return (rbs_node_t *)rbs_ast_members_instancevariable_new(((rbs_node_t *)name)->cached_ruby_value, type, location, comment);

  case tA2IDENT:
    name_range = state->current_token.range;
    name = rbs_ast_symbol_new(ID2SYM(INTERN_TOKEN(state, state->current_token)));

    parser_advance_assert(state, pCOLON);
    colon_range = state->current_token.range;

    parser_push_typevar_table(state, true);
    type = parse_type(state);
    parser_pop_typevar_table(state);
    member_range.end = state->current_token.range.end;

    location = rbs_new_location(state->buffer, member_range);
    loc = rbs_check_location(location);
    rbs_loc_alloc_children(loc, 3);
    rbs_loc_add_required_child(loc, rb_intern("name"), name_range);
    rbs_loc_add_required_child(loc, rb_intern("colon"), colon_range);
    rbs_loc_add_optional_child(loc, rb_intern("kind"), kind_range);

    return (rbs_node_t *) rbs_ast_members_classvariable_new(((rbs_node_t *)name)->cached_ruby_value, type, location, comment);

  case kSELF:
    kind_range.start = state->current_token.range.start;
    kind_range.end = state->next_token.range.end;

    parser_advance_assert(state, pDOT);
    parser_advance_assert(state, tAIDENT);

    name_range = state->current_token.range;
    name = rbs_ast_symbol_new(ID2SYM(INTERN_TOKEN(state, state->current_token)));

    parser_advance_assert(state, pCOLON);
    colon_range = state->current_token.range;

    parser_push_typevar_table(state, true);
    type = parse_type(state);
    parser_pop_typevar_table(state);
    member_range.end = state->current_token.range.end;

    location = rbs_new_location(state->buffer, member_range);
    loc = rbs_check_location(location);
    rbs_loc_alloc_children(loc, 3);
    rbs_loc_add_required_child(loc, rb_intern("name"), name_range);
    rbs_loc_add_required_child(loc, rb_intern("colon"), colon_range);
    rbs_loc_add_optional_child(loc, rb_intern("kind"), kind_range);

    return (rbs_node_t *)rbs_ast_members_classinstancevariable_new(((rbs_node_t *)name)->cached_ruby_value, type, location, comment);

  default:
    rbs_abort();
  }
}

/*
  visibility_member ::= {<`public`>}
                      | {<`private`>}
*/
rbs_node_t *parse_visibility_member(parserstate *state, rbs_node_list_t *annotations) {
  if (annotations->length > 0) {
    raise_syntax_error(
      state,
      state->current_token,
      "annotation cannot be given to visibility members"
    );
  }

  VALUE location = rbs_new_location(state->buffer, state->current_token.range);

  switch (state->current_token.type)
  {
  case kPUBLIC:
    return (rbs_node_t *) rbs_ast_members_public_new(location);
  case kPRIVATE:
    return (rbs_node_t *) rbs_ast_members_private_new(location);
  default:
    rbs_abort();
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
rbs_node_t *parse_attribute_member(parserstate *state, position comment_pos, rbs_node_list_t *annotations) {
  range member_range;
  range keyword_range, name_range, colon_range;
  range kind_range = NULL_RANGE, ivar_range = NULL_RANGE, ivar_name_range = NULL_RANGE, visibility_range = NULL_RANGE;

  InstanceSingletonKind is_kind;
  VALUE kind;
  rbs_ast_symbol_t *attr_name;
  VALUE ivar_name;
  rbs_node_t *type;
  VALUE comment;
  VALUE location;
  rbs_ast_symbol_t *visibility;
  rbs_loc *loc;
  enum TokenType attr_type;

  member_range.start = state->current_token.range.start;
  comment_pos = nonnull_pos_or(comment_pos, member_range.start);
  comment = get_comment(state, comment_pos.line);

  switch (state->current_token.type)
  {
  case kPRIVATE:
    visibility = rbs_ast_symbol_new(ID2SYM(rb_intern("private")));
    visibility_range = state->current_token.range;
    parser_advance(state);
    break;
  case kPUBLIC:
    visibility = rbs_ast_symbol_new(ID2SYM(rb_intern("public")));
    visibility_range = state->current_token.range;
    parser_advance(state);
    break;
  default:
    visibility = NULL;
    visibility_range = NULL_RANGE;
    break;
  }

  attr_type = state->current_token.type;
  keyword_range = state->current_token.range;

  is_kind = parse_instance_singleton_kind(state, false, &kind_range);
  if (is_kind == INSTANCE_KIND) {
    kind = ID2SYM(rb_intern("instance"));
  } else {
    kind = ID2SYM(rb_intern("singleton"));
  }

  attr_name = parse_method_name(state, &name_range);

  if (state->next_token.type == pLPAREN) {
    parser_advance_assert(state, pLPAREN);
    ivar_range.start = state->current_token.range.start;

    if (parser_advance_if(state, tAIDENT)) {
      ivar_name = ID2SYM(INTERN_TOKEN(state, state->current_token));
      ivar_name_range = state->current_token.range;
    } else {
      ivar_name = Qfalse;
    }

    parser_advance_assert(state, pRPAREN);
    ivar_range.end = state->current_token.range.end;
  } else {
    ivar_name = Qnil;
  }

  parser_advance_assert(state, pCOLON);
  colon_range = state->current_token.range;

  parser_push_typevar_table(state, is_kind == SINGLETON_KIND);
  type = parse_type(state);
  parser_pop_typevar_table(state);
  member_range.end = state->current_token.range.end;

  location = rbs_new_location(state->buffer, member_range);
  loc = rbs_check_location(location);
  rbs_loc_alloc_children(loc, 7);
  rbs_loc_add_required_child(loc, rb_intern("keyword"), keyword_range);
  rbs_loc_add_required_child(loc, rb_intern("name"), name_range);
  rbs_loc_add_required_child(loc, rb_intern("colon"), colon_range);
  rbs_loc_add_optional_child(loc, rb_intern("kind"), kind_range);
  rbs_loc_add_optional_child(loc, rb_intern("ivar"), ivar_range);
  rbs_loc_add_optional_child(loc, rb_intern("ivar_name"), ivar_name_range);
  rbs_loc_add_optional_child(loc, rb_intern("visibility"), visibility_range);

  VALUE visibility_value = visibility ? ((rbs_node_t *)visibility)->cached_ruby_value : Qnil;

  switch (attr_type)
  {
  case kATTRREADER:
    return (rbs_node_t *) rbs_ast_members_attrreader_new(((rbs_node_t *)attr_name)->cached_ruby_value, type, ivar_name, kind, annotations, location, comment, visibility_value);
  case kATTRWRITER:
    return (rbs_node_t *) rbs_ast_members_attrwriter_new(((rbs_node_t *)attr_name)->cached_ruby_value, type, ivar_name, kind, annotations, location, comment, visibility_value);
  case kATTRACCESSOR:
    return (rbs_node_t *) rbs_ast_members_attraccessor_new(((rbs_node_t *)attr_name)->cached_ruby_value, type, ivar_name, kind, annotations, location, comment, visibility_value);
  default:
    rbs_abort();
  }
}

/*
  interface_members ::= {} ...<interface_member> kEND

  interface_member ::= def_member     (instance method only && no overloading)
                     | mixin_member   (interface only)
                     | alias_member   (instance only)
*/
rbs_node_list_t *parse_interface_members(parserstate *state) {
  rbs_node_list_t *members = rbs_node_list_new();

  while (state->next_token.type != kEND) {
    rbs_node_list_t *annotations = rbs_node_list_new();
    position annot_pos = NullPosition;

    parse_annotations(state, annotations, &annot_pos);

    parser_advance(state);

    rbs_node_t *member;
    switch (state->current_token.type) {
    case kDEF:
      member = (rbs_node_t *) parse_member_def(state, true, true, annot_pos, annotations);
      break;

    case kINCLUDE:
    case kEXTEND:
    case kPREPEND:
      member = (rbs_node_t *) parse_mixin_member(state, true, annot_pos, annotations);
      break;

    case kALIAS:
      member = (rbs_node_t *) parse_alias_member(state, true, annot_pos, annotations);
      break;

    default:
      raise_syntax_error(
        state,
        state->current_token,
        "unexpected token for interface declaration member"
      );
    }

    rbs_node_list_append(members, member);
  }

  return members;
}

/*
  interface_decl ::= {`interface`} interface_name module_type_params interface_members <kEND>
*/
rbs_ast_declarations_interface_t *parse_interface_decl(parserstate *state, position comment_pos, rbs_node_list_t *annotations) {
  range member_range;
  range name_range, keyword_range, end_range;
  range type_params_range = NULL_RANGE;

  member_range.start = state->current_token.range.start;
  comment_pos = nonnull_pos_or(comment_pos, member_range.start);

  parser_push_typevar_table(state, true);
  keyword_range = state->current_token.range;

  parser_advance(state);

  rbs_typename_t *name = parse_type_name(state, INTERFACE_NAME, &name_range);
  rbs_node_list_t *type_params = parse_type_params(state, &type_params_range, true);
  rbs_node_list_t *members = parse_interface_members(state);

  parser_advance_assert(state, kEND);
  end_range = state->current_token.range;
  member_range.end = end_range.end;

  parser_pop_typevar_table(state);

  VALUE location = rbs_new_location(state->buffer, member_range);
  rbs_loc *loc = rbs_check_location(location);
  rbs_loc_alloc_children(loc, 4);
  rbs_loc_add_required_child(loc, rb_intern("keyword"), keyword_range);
  rbs_loc_add_required_child(loc, rb_intern("name"), name_range);
  rbs_loc_add_required_child(loc, rb_intern("end"), end_range);
  rbs_loc_add_optional_child(loc, rb_intern("type_params"), type_params_range);

  VALUE comment = get_comment(state, comment_pos.line);

  return rbs_ast_declarations_interface_new(name, type_params, members, annotations, location, comment);
}

/*
  module_self_types ::= {`:`} module_self_type `,` ... `,` <module_self_type>

  module_self_type ::= <module_name>
                     | module_name `[` type_list <`]`>
*/
void parse_module_self_types(parserstate *state, rbs_node_list_t *array) {
  while (true) {
    range self_range;
    range name_range;
    range args_range = NULL_RANGE;

    parser_advance(state);

    self_range.start = state->current_token.range.start;

    rbs_typename_t *module_name = parse_type_name(state, CLASS_NAME | INTERFACE_NAME, &name_range);
    self_range.end = name_range.end;

    rbs_node_list_t *args = rbs_node_list_new();
    if (state->next_token.type == pLBRACKET) {
      parser_advance(state);
      args_range.start = state->current_token.range.start;
      parse_type_list(state, pRBRACKET, args);
      parser_advance(state);
      self_range.end = args_range.end = state->current_token.range.end;
    }

    VALUE location = rbs_new_location(state->buffer, self_range);
    rbs_loc *loc = rbs_check_location(location);
    rbs_loc_alloc_children(loc, 2);
    rbs_loc_add_required_child(loc, rb_intern("name"), name_range);
    rbs_loc_add_optional_child(loc, rb_intern("args"), args_range);

    VALUE value = rbs_ast_decl_module_self(((rbs_node_t *)module_name)->cached_ruby_value, args->cached_ruby_value, location);
    rbs_ast_declarations_module_self_t *self_type = rbs_ast_declarations_module_self_new(value, ((rbs_node_t *)module_name)->cached_ruby_value, args->cached_ruby_value, location);
    rbs_node_list_append(array, (rbs_node_t *)self_type);

    if (state->next_token.type == pCOMMA) {
      parser_advance(state);
    } else {
      break;
    }
  }
}

rbs_node_t *parse_nested_decl(parserstate *state, const char *nested_in, position annot_pos, rbs_node_list_t *annotations);

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
rbs_node_list_t *parse_module_members(parserstate *state) {
  rbs_node_list_t *members = rbs_node_list_new();

  while (state->next_token.type != kEND) {
    rbs_node_t *member;
    rbs_node_list_t *annotations = rbs_node_list_new();
    position annot_pos = NullPosition;

    parse_annotations(state, annotations, &annot_pos);

    parser_advance(state);

    switch (state->current_token.type)
    {
    case kDEF:
      member = (rbs_node_t *) parse_member_def(state, false, true, annot_pos, annotations);
      break;

    case kINCLUDE:
    case kEXTEND:
    case kPREPEND:
      member = (rbs_node_t *) parse_mixin_member(state, false, annot_pos, annotations);
      break;

    case kALIAS:
      member = (rbs_node_t *) parse_alias_member(state, false, annot_pos, annotations);
      break;

    case tAIDENT:
    case tA2IDENT:
    case kSELF:
      member = parse_variable_member(state, annot_pos, annotations);
      break;

    case kATTRREADER:
    case kATTRWRITER:
    case kATTRACCESSOR:
      member = parse_attribute_member(state, annot_pos, annotations);
      break;

    case kPUBLIC:
    case kPRIVATE:
      if (state->next_token.range.start.line == state->current_token.range.start.line) {
        switch (state->next_token.type)
        {
        case kDEF:
          member = (rbs_node_t *) parse_member_def(state, false, true, annot_pos, annotations);
          break;
        case kATTRREADER:
        case kATTRWRITER:
        case kATTRACCESSOR:
          member = parse_attribute_member(state, annot_pos, annotations);
          break;
        default:
          raise_syntax_error(state, state->next_token, "method or attribute definition is expected after visibility modifier");
        }
      } else {
        member = parse_visibility_member(state, annotations);
      }
      break;

    default:
      member = parse_nested_decl(state, "module", annot_pos, annotations);
      break;
    }

    rbs_node_list_append(members, member);
  }

  return members;
}

/*
  module_decl ::= {module_name} module_type_params module_members <kEND>
                | {module_name} module_name module_type_params `:` module_self_types module_members <kEND>
*/
rbs_ast_declarations_module_t *parse_module_decl0(parserstate *state, range keyword_range, rbs_typename_t *module_name, range name_range, VALUE comment, rbs_node_list_t *annotations) {
  range decl_range;
  range end_range;
  range type_params_range;
  range colon_range;
  range self_types_range;

  parser_push_typevar_table(state, true);

  decl_range.start = keyword_range.start;

  rbs_node_list_t *type_params = parse_type_params(state, &type_params_range, true);
  rbs_node_list_t *self_types = rbs_node_list_new();

  if (state->next_token.type == pCOLON) {
    parser_advance(state);
    colon_range = state->current_token.range;
    self_types_range.start = state->next_token.range.start;
    parse_module_self_types(state, self_types);
    self_types_range.end = state->current_token.range.end;
  } else {
    colon_range = NULL_RANGE;
    self_types_range = NULL_RANGE;
  }

  rbs_node_list_t *members = parse_module_members(state);

  parser_advance_assert(state, kEND);
  end_range = state->current_token.range;
  decl_range.end = state->current_token.range.end;

  VALUE location = rbs_new_location(state->buffer, decl_range);
  rbs_loc *loc = rbs_check_location(location);
  rbs_loc_alloc_children(loc, 6);
  rbs_loc_add_required_child(loc, rb_intern("keyword"), keyword_range);
  rbs_loc_add_required_child(loc, rb_intern("name"), name_range);
  rbs_loc_add_required_child(loc, rb_intern("end"), end_range);
  rbs_loc_add_optional_child(loc, rb_intern("type_params"), type_params_range);
  rbs_loc_add_optional_child(loc, rb_intern("colon"), colon_range);
  rbs_loc_add_optional_child(loc, rb_intern("self_types"), self_types_range);

  parser_pop_typevar_table(state);

  return rbs_ast_declarations_module_new(((rbs_node_t *)module_name)->cached_ruby_value, type_params, self_types->cached_ruby_value, members, annotations, location, comment);
}

/*
  module_decl ::= {`module`} module_name `=` old_module_name <kEND>
                | {`module`} module_name module_decl0 <kEND>

*/
rbs_node_t *parse_module_decl(parserstate *state, position comment_pos, rbs_node_list_t *annotations) {
  range keyword_range = state->current_token.range;
  range module_name_range;

  comment_pos = nonnull_pos_or(comment_pos, state->current_token.range.start);
  VALUE comment = get_comment(state, comment_pos.line);

  parser_advance(state);
  rbs_typename_t *module_name = parse_type_name(state, CLASS_NAME, &module_name_range);

  if (state->next_token.type == pEQ) {
    range eq_range = state->next_token.range;
    parser_advance(state);
    parser_advance(state);

    range old_name_range;
    rbs_typename_t *old_name = parse_type_name(state, CLASS_NAME, &old_name_range);

    range decl_range;
    decl_range.start = keyword_range.start;
    decl_range.end = old_name_range.end;

    VALUE location = rbs_new_location(state->buffer, decl_range);
    rbs_loc *loc = rbs_check_location(location);
    rbs_loc_alloc_children(loc, 4);
    rbs_loc_add_required_child(loc, rb_intern("keyword"), keyword_range);
    rbs_loc_add_required_child(loc, rb_intern("new_name"), module_name_range);
    rbs_loc_add_required_child(loc, rb_intern("eq"), eq_range);
    rbs_loc_add_optional_child(loc, rb_intern("old_name"), old_name_range);

    return (rbs_node_t *) rbs_ast_declarations_modulealias_new(module_name, old_name, location, comment);
  } else {
    return (rbs_node_t *) parse_module_decl0(state, keyword_range, module_name, module_name_range, comment, annotations);
  }
}

/*
  class_decl_super ::= {} `<` <class_instance_name>
                     | {<>}
*/
rbs_ast_declarations_class_super_t *parse_class_decl_super(parserstate *state, range *lt_range) {
  if (parser_advance_if(state, pLT)) {
    range super_range;
    range name_range;
    range args_range = NULL_RANGE;

    VALUE name;
    rbs_node_list_t *args;
    VALUE location;
    rbs_loc *loc;

    *lt_range = state->current_token.range;
    super_range.start = state->next_token.range.start;

    args = rbs_node_list_new();
    class_instance_name(state, CLASS_NAME, &name, args, &name_range, &args_range);

    super_range.end = state->current_token.range.end;

    location = rbs_new_location(state->buffer, super_range);
    loc = rbs_check_location(location);
    rbs_loc_alloc_children(loc, 2);
    rbs_loc_add_required_child(loc, rb_intern("name"), name_range);
    rbs_loc_add_optional_child(loc, rb_intern("args"), args_range);

    return rbs_ast_declarations_class_super_new(name, args->cached_ruby_value, location);
  } else {
    *lt_range = NULL_RANGE;
    return NULL;
  }
}

/*
  class_decl ::= {class_name} type_params class_decl_super class_members <`end`>
*/
rbs_ast_declarations_class_t *parse_class_decl0(parserstate *state, range keyword_range, rbs_typename_t *name, range name_range, VALUE comment, rbs_node_list_t *annotations) {
  range decl_range;
  range end_range;
  range type_params_range;
  range lt_range;

  rbs_node_list_t *type_params;
  VALUE super;
  rbs_node_list_t *members;
  VALUE location;

  rbs_loc *loc;

  parser_push_typevar_table(state, true);

  decl_range.start = keyword_range.start;

  type_params = parse_type_params(state, &type_params_range, true);

  rbs_ast_declarations_class_super_t *super_node = parse_class_decl_super(state, &lt_range);
  if (super_node != NULL) {
    super = ((rbs_node_t *)super_node)->cached_ruby_value;
  } else {
    super = Qnil;
  }

  members = parse_module_members(state);
  parser_advance_assert(state, kEND);
  end_range = state->current_token.range;

  decl_range.end = end_range.end;

  parser_pop_typevar_table(state);

  location = rbs_new_location(state->buffer, decl_range);
  loc = rbs_check_location(location);
  rbs_loc_alloc_children(loc, 5);
  rbs_loc_add_required_child(loc, rb_intern("keyword"), keyword_range);
  rbs_loc_add_required_child(loc, rb_intern("name"), name_range);
  rbs_loc_add_required_child(loc, rb_intern("end"), end_range);
  rbs_loc_add_optional_child(loc, rb_intern("type_params"), type_params_range);
  rbs_loc_add_optional_child(loc, rb_intern("lt"), lt_range);

  return rbs_ast_declarations_class_new(((rbs_node_t *)name)->cached_ruby_value, type_params, super, members, annotations, location, comment);
}

/*
  class_decl ::= {`class`} class_name `=` <class_name>
               | {`class`} class_name <class_decl0>
*/
rbs_node_t *parse_class_decl(parserstate *state, position comment_pos, rbs_node_list_t *annotations) {
  range keyword_range = state->current_token.range;
  range class_name_range;

  comment_pos = nonnull_pos_or(comment_pos, state->current_token.range.start);
  VALUE comment = get_comment(state, comment_pos.line);

  parser_advance(state);
  rbs_typename_t *class_name = parse_type_name(state, CLASS_NAME, &class_name_range);

  if (state->next_token.type == pEQ) {
    range eq_range = state->next_token.range;
    parser_advance(state);
    parser_advance(state);

    range old_name_range;
    rbs_typename_t *old_name = parse_type_name(state, CLASS_NAME, &old_name_range);

    range decl_range;
    decl_range.start = keyword_range.start;
    decl_range.end = old_name_range.end;

    VALUE location = rbs_new_location(state->buffer, decl_range);
    rbs_loc *loc = rbs_check_location(location);
    rbs_loc_alloc_children(loc, 4);
    rbs_loc_add_required_child(loc, rb_intern("keyword"), keyword_range);
    rbs_loc_add_required_child(loc, rb_intern("new_name"), class_name_range);
    rbs_loc_add_required_child(loc, rb_intern("eq"), eq_range);
    rbs_loc_add_optional_child(loc, rb_intern("old_name"), old_name_range);


    return (rbs_node_t *) rbs_ast_declarations_classalias_new(class_name, old_name, location, comment);
  } else {
    return (rbs_node_t *) parse_class_decl0(state, keyword_range, class_name, class_name_range, comment, annotations);
  }
}

/*
  nested_decl ::= {<const_decl>}
                | {<class_decl>}
                | {<interface_decl>}
                | {<module_decl>}
                | {<class_decl>}
*/
rbs_node_t *parse_nested_decl(parserstate *state, const char *nested_in, position annot_pos, rbs_node_list_t *annotations) {
  rbs_node_t *decl;

  parser_push_typevar_table(state, true);

  switch (state->current_token.type) {
  case tUIDENT:
  case pCOLON2:
    decl = (rbs_node_t *) parse_const_decl(state);
    break;
  case tGIDENT:
    decl = (rbs_node_t *) parse_global_decl(state);
    break;
  case kTYPE:
    decl = (rbs_node_t *) parse_type_decl(state, annot_pos, annotations);
    break;
  case kINTERFACE:
    decl = (rbs_node_t *) parse_interface_decl(state, annot_pos, annotations);
    break;
  case kMODULE:
    decl = (rbs_node_t *) parse_module_decl(state, annot_pos, annotations);
    break;
  case kCLASS:
    decl = (rbs_node_t *) parse_class_decl(state, annot_pos, annotations);
    break;
  default:
    raise_syntax_error(
      state,
      state->current_token,
      "unexpected token for class/module declaration member"
    );
  }

  parser_pop_typevar_table(state);

  return decl;
}

rbs_node_t *parse_decl(parserstate *state) {
  rbs_node_list_t *annotations = rbs_node_list_new();
  position annot_pos = NullPosition;

  parse_annotations(state, annotations, &annot_pos);

  parser_advance(state);
  switch (state->current_token.type) {
  case tUIDENT:
  case pCOLON2:
    return (rbs_node_t *) parse_const_decl(state);
  case tGIDENT:
    return (rbs_node_t *) parse_global_decl(state);
  case kTYPE:
    return (rbs_node_t *) parse_type_decl(state, annot_pos, annotations);
  case kINTERFACE:
    return (rbs_node_t *) parse_interface_decl(state, annot_pos, annotations);
  case kMODULE:
    return (rbs_node_t *) parse_module_decl(state, annot_pos, annotations);
  case kCLASS:
    return (rbs_node_t *) parse_class_decl(state, annot_pos, annotations);
  default:
    raise_syntax_error(
      state,
      state->current_token,
      "cannot start a declaration"
    );
  }
}

/*
  namespace ::= {} (`::`)? (`tUIDENT` `::`)* `tUIDENT` <`::`>
              | {} <>                                            (empty -- returns empty namespace)
*/
rbs_namespace_t *parse_namespace(parserstate *state, range *rg) {
  bool is_absolute = false;

  if (state->next_token.type == pCOLON2) {
    rg->start = state->next_token.range.start;
    rg->end = state->next_token.range.end;
    is_absolute = true;

    parser_advance(state);
  }

  rbs_node_list_t *path = rbs_node_list_new();

  while (true) {
    if (state->next_token.type == tUIDENT && state->next_token2.type == pCOLON2) {
      VALUE symbol_value = ID2SYM(INTERN_TOKEN(state, state->next_token));
      rbs_ast_symbol_t *symbol = rbs_ast_symbol_new(symbol_value);
      rbs_node_list_append(path, (rbs_node_t *)symbol);
      if (null_position_p(rg->start)) {
        rg->start = state->next_token.range.start;
      }
      rg->end = state->next_token2.range.end;
      parser_advance(state);
      parser_advance(state);
    } else {
      break;
    }
  }

  return rbs_namespace_new(path->cached_ruby_value, is_absolute ? Qtrue : Qfalse);
}

/*
  use_clauses ::= {} use_clause `,` ... `,` <use_clause>

  use_clause ::= {} namespace <tUIDENT>
               | {} namespace tUIDENT `as` <tUIDENT>
               | {} namespace <tSTAR>
*/
void parse_use_clauses(parserstate *state, VALUE clauses) {
  while (true) {
    range namespace_range = NULL_RANGE;
    rbs_namespace_t *namespace = parse_namespace(state, &namespace_range);

    range clause_range = namespace_range;

    switch (state->next_token.type)
    {
      case tLIDENT:
      case tULIDENT:
      case tUIDENT: {
        parser_advance(state);

        enum TokenType ident_type = state->current_token.type;

        range type_name_range;
        if (null_range_p(namespace_range)) {
          type_name_range = state->current_token.range;
        } else {
          type_name_range.start = namespace_range.start;
          type_name_range.end = state->current_token.range.end;
        }
        clause_range = type_name_range;

        rbs_ast_symbol_t *symbol = rbs_ast_symbol_new(ID2SYM(INTERN_TOKEN(state, state->current_token)));
        VALUE type_name = rbs_type_name(((rbs_node_t *)namespace)->cached_ruby_value, ((rbs_node_t *)symbol)->cached_ruby_value);

        range keyword_range = NULL_RANGE;
        range new_name_range = NULL_RANGE;

        VALUE new_name = Qnil;
        if (state->next_token.type == kAS) {
          parser_advance(state);
          keyword_range = state->current_token.range;

          if (ident_type == tUIDENT) parser_advance_assert(state, tUIDENT);
          if (ident_type == tLIDENT) parser_advance_assert(state, tLIDENT);
          if (ident_type == tULIDENT) parser_advance_assert(state, tULIDENT);

          new_name = ID2SYM(INTERN_TOKEN(state, state->current_token));
          new_name_range = state->current_token.range;
          clause_range.end = new_name_range.end;
        }

        VALUE location = rbs_new_location(state->buffer, clause_range);
        rbs_loc *loc = rbs_check_location(location);
        rbs_loc_alloc_children(loc, 3);
        rbs_loc_add_required_child(loc, rb_intern("type_name"), type_name_range);
        rbs_loc_add_optional_child(loc, rb_intern("keyword"), keyword_range);
        rbs_loc_add_optional_child(loc, rb_intern("new_name"), new_name_range);

        rb_ary_push(clauses, rbs_ast_directives_use_single_clause(type_name, new_name, location));

        break;
      }
      case pSTAR:
      {
        parser_advance(state);

        range star_range = state->current_token.range;
        clause_range.end = star_range.end;

        VALUE location = rbs_new_location(state->buffer, clause_range);
        rbs_loc *loc = rbs_check_location(location);
        rbs_loc_alloc_children(loc, 2);
        rbs_loc_add_required_child(loc, rb_intern("namespace"), namespace_range);
        rbs_loc_add_required_child(loc, rb_intern("star"), star_range);

        rb_ary_push(clauses, rbs_ast_directives_use_wildcard_clause(((rbs_node_t *)namespace)->cached_ruby_value, location));

        break;
      }
      default:
        raise_syntax_error(
          state,
          state->next_token,
          "use clause is expected"
        );
    }

    if (state->next_token.type == pCOMMA) {
      parser_advance(state);
    } else {
      break;
    }
  }

  return;
}

/*
  use_directive ::= {} `use` <clauses>
 */
rbs_ast_directives_use_t *parse_use_directive(parserstate *state) {
  if (state->next_token.type == kUSE) {
    parser_advance(state);

    range keyword_range = state->current_token.range;

    VALUE clauses = rb_ary_new();
    parse_use_clauses(state, clauses);

    range directive_range = keyword_range;
    directive_range.end = state->current_token.range.end;

    VALUE location = rbs_new_location(state->buffer, directive_range);
    rbs_loc *loc = rbs_check_location(location);
    rbs_loc_alloc_children(loc, 1);
    rbs_loc_add_required_child(loc, rb_intern("keyword"), keyword_range);

    return rbs_ast_directives_use_new(clauses, location);
  } else {
    return NULL;
  }
}

rbs_node_list_t *parse_signature(parserstate *state) {
  rbs_node_list_t *dir_nodes = rbs_node_list_new();
  rbs_ast_directives_nodes_t *dirs = rbs_ast_directives_nodes_new(dir_nodes->cached_ruby_value, dir_nodes->cached_ruby_value);

  rbs_node_list_t *decl_nodes = rbs_node_list_new();
  rbs_ast_declarations_nodes_t *decls = rbs_ast_declarations_nodes_new(decl_nodes->cached_ruby_value, decl_nodes->cached_ruby_value);

  while (state->next_token.type == kUSE) {
    rbs_ast_directives_use_t *use_node = parse_use_directive(state);
    if (use_node == NULL) {
      rbs_node_list_append(dir_nodes, NULL);
    } else {
      rbs_node_list_append(dir_nodes, (rbs_node_t *)use_node);
    }
  }

  while (state->next_token.type != pEOF) {
    rbs_node_t *decl = parse_decl(state);
    rbs_node_list_append(decl_nodes, decl);
  }

  rbs_node_list_t *ret = rbs_node_list_new();
  rbs_node_list_append(ret, (rbs_node_t *)dirs);
  rbs_node_list_append(ret, (rbs_node_t *)decls);
  return ret;
}

struct parse_type_arg {
  parserstate *parser;
  VALUE require_eof;
};

static VALUE
ensure_free_parser(VALUE parser) {
  free_parser((parserstate *)parser);
  return Qnil;
}

static VALUE
parse_type_try(VALUE a) {
  struct parse_type_arg *arg = (struct parse_type_arg *)a;

  if (arg->parser->next_token.type == pEOF) {
    return Qnil;
  }

  rbs_node_t *type = parse_type(arg->parser);

  if (RB_TEST(arg->require_eof)) {
    parser_advance_assert(arg->parser, pEOF);
  }

  return type->cached_ruby_value;
}

static VALUE
rbsparser_parse_type(VALUE self, VALUE buffer, VALUE start_pos, VALUE end_pos, VALUE variables, VALUE require_eof)
{
  VALUE string = rb_funcall(buffer, rb_intern("content"), 0);
  StringValue(string);
  lexstate *lexer = alloc_lexer(string, FIX2INT(start_pos), FIX2INT(end_pos));
  parserstate *parser = alloc_parser(buffer, lexer, FIX2INT(start_pos), FIX2INT(end_pos), variables);
  struct parse_type_arg arg = {
    parser,
    require_eof
  };
  return rb_ensure(parse_type_try, (VALUE)&arg, ensure_free_parser, (VALUE)parser);
}

static VALUE
parse_method_type_try(VALUE a) {
  struct parse_type_arg *arg = (struct parse_type_arg *)a;

  if (arg->parser->next_token.type == pEOF) {
    return Qnil;
  }

  rbs_methodtype_t *method_type = parse_method_type(arg->parser);

  if (RB_TEST(arg->require_eof)) {
    parser_advance_assert(arg->parser, pEOF);
  }

  return rbs_struct_to_ruby_value((rbs_node_t *)method_type);
}

static VALUE
rbsparser_parse_method_type(VALUE self, VALUE buffer, VALUE start_pos, VALUE end_pos, VALUE variables, VALUE require_eof)
{
  VALUE string = rb_funcall(buffer, rb_intern("content"), 0);
  StringValue(string);
  lexstate *lexer = alloc_lexer(string, FIX2INT(start_pos), FIX2INT(end_pos));
  parserstate *parser = alloc_parser(buffer, lexer, FIX2INT(start_pos), FIX2INT(end_pos), variables);
  struct parse_type_arg arg = {
    parser,
    require_eof
  };
  return rb_ensure(parse_method_type_try, (VALUE)&arg, ensure_free_parser, (VALUE)parser);
}

static VALUE
parse_signature_try(VALUE a) {
  parserstate *parser = (parserstate *)a;
  return parse_signature(parser)->cached_ruby_value;
}

static VALUE
rbsparser_parse_signature(VALUE self, VALUE buffer, VALUE end_pos)
{
  VALUE string = rb_funcall(buffer, rb_intern("content"), 0);
  StringValue(string);
  lexstate *lexer = alloc_lexer(string, 0, FIX2INT(end_pos));
  parserstate *parser = alloc_parser(buffer, lexer, 0, FIX2INT(end_pos), Qnil);
  return rb_ensure(parse_signature_try, (VALUE)parser, ensure_free_parser, (VALUE)parser);
}

static VALUE
rbsparser_lex(VALUE self, VALUE buffer, VALUE end_pos) {
  VALUE string = rb_funcall(buffer, rb_intern("content"), 0);
  StringValue(string);
  lexstate *lexer = alloc_lexer(string, 0, FIX2INT(end_pos));
  VALUE results = rb_ary_new();

  token token = NullToken;
  while (token.type != pEOF) {
    token = rbsparser_next_token(lexer);
    VALUE type = ID2SYM(rb_intern(token_type_str(token.type)));
    VALUE location = rbs_new_location(buffer, token.range);
    VALUE pair = rb_ary_new3(2, type, location);
    rb_ary_push(results, pair);
  }

  free(lexer);

  return results;
}

void rbs__init_parser(void) {
  RBS_Parser = rb_define_class_under(RBS, "Parser", rb_cObject);
  rb_gc_register_mark_object(RBS_Parser);
  VALUE empty_array = rb_obj_freeze(rb_ary_new());
  rb_gc_register_mark_object(empty_array);

  rb_define_singleton_method(RBS_Parser, "_parse_type", rbsparser_parse_type, 5);
  rb_define_singleton_method(RBS_Parser, "_parse_method_type", rbsparser_parse_method_type, 5);
  rb_define_singleton_method(RBS_Parser, "_parse_signature", rbsparser_parse_signature, 2);
  rb_define_singleton_method(RBS_Parser, "_lex", rbsparser_lex, 2);
}
