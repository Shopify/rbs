#include "rbs_extension.h"

#include "rbs/encoding.h"
#include "rbs/rbs_buffer.h"

#define RESET_TABLE_P(table) (table->size == 0)

id_table *alloc_empty_table(void) {
  id_table *table = malloc(sizeof(id_table));

  *table = (id_table) {
    .size = 10,
    .count = 0,
    .ids = calloc(10, sizeof(rbs_constant_id_t)),
    .next = NULL,
  };

  return table;
}

id_table *alloc_reset_table(void) {
  id_table *table = malloc(sizeof(id_table));

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
    id_table *table = alloc_reset_table();
    table->next = state->vars;
    state->vars = table;
  }

  id_table *table = alloc_empty_table();
  table->next = state->vars;
  state->vars = table;

  return table;
}

void parser_pop_typevar_table(parserstate *state) {
  id_table *table;

  if (state->vars) {
    table = state->vars;
    state->vars = table->next;
    free(table->ids);
    free(table);
  } else {
    rb_raise(rb_eRuntimeError, "Cannot pop empty table");
  }

  if (state->vars && RESET_TABLE_P(state->vars)) {
    table = state->vars;
    state->vars = table->next;
    free(table);
  }
}

void parser_insert_typevar(parserstate *state, rbs_constant_id_t id) {
  id_table *table = state->vars;

  if (RESET_TABLE_P(table)) {
    rb_raise(rb_eRuntimeError, "Cannot insert to reset table");
  }

  if (table->size == table->count) {
    // expand
    rbs_constant_id_t *ptr = table->ids;
    table->size += 10;
    table->ids = calloc(table->size, sizeof(rbs_constant_id_t));
    memcpy(table->ids, ptr, sizeof(rbs_constant_id_t) * table->count);
    free(ptr);
  }

  table->ids[table->count++] = id;
}

bool parser_typevar_member(parserstate *state, rbs_constant_id_t id) {
  id_table *table = state->vars;

  while (table && !RESET_TABLE_P(table)) {
    for (size_t i = 0; i < table->count; i++) {
      if (rbs_constant_id_equal(fake_constant_pool, table->ids[i], id)) {
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

/**
 * Advance token if _next_ token is `type`.
 * Ensures one token advance and `state->current_token.type == type`, or current token not changed.
 *
 * @returns true if token advances, false otherwise.
 **/
bool parser_advance_if(parserstate *state, enum TokenType type) {
  if (state->next_token.type == type) {
    parser_advance(state);
    return true;
  } else {
    return false;
  }
}

void print_token(token tok) {
  printf(
    "%s char=%d...%d\n",
    token_type_str(tok.type),
    tok.range.start.char_pos,
    tok.range.end.char_pos
  );
}

void insert_comment_line(parserstate *state, token tok) {
  int prev_line = tok.range.start.line - 1;

  comment *com = comment_get_comment(state->last_comment, prev_line);

  if (com) {
    comment_insert_new_line(com, tok);
  } else {
    state->last_comment = alloc_comment(tok, state->last_comment);
  }
}

static rbs_ast_comment_t *parse_comment_lines(parserstate *state, comment *com) {
  size_t hash_bytes = state->lexstate->encoding->char_width((const uint8_t *) "#", (size_t) 1);
  size_t space_bytes = state->lexstate->encoding->char_width((const uint8_t *) " ", (size_t) 1);

  rbs_buffer_t rbs_buffer;
  rbs_buffer_init(&rbs_buffer);

  for (size_t i = 0; i < com->line_count; i++) {
    token tok = com->tokens[i];

    const char *comment_start = state->lexstate->string.start + tok.range.start.byte_pos + hash_bytes;
    size_t comment_bytes = RANGE_BYTES(tok.range) - hash_bytes;

    rbs_string_t str = {
      .start = comment_start,
      .end = state->lexstate->string.end,
      .type = RBS_STRING_SHARED,
    };
    unsigned char c = utf8_to_codepoint(str);

    if (c == ' ') {
      comment_start += space_bytes;
      comment_bytes -= space_bytes;
    }

    rbs_buffer_append_string(&rbs_buffer, comment_start, comment_bytes);
    rbs_buffer_append_cstr(&rbs_buffer, "\n");
  }

  return rbs_ast_comment_new(
    rbs_buffer_to_string(&rbs_buffer),
    rbs_location_pp(&com->start, &com->end)
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

comment *alloc_comment(token comment_token, comment *last_comment) {
  comment *new_comment = malloc(sizeof(comment));

  *new_comment = (comment) {
    .start = comment_token.range.start,
    .end = comment_token.range.end,

    .line_size = 0,
    .line_count = 0,
    .tokens = NULL,

    .next_comment = last_comment,
  };

  comment_insert_new_line(new_comment, comment_token);

  return new_comment;
}

void free_comment(comment *com) {
  if (com->next_comment) {
    free_comment(com->next_comment);
  }

  free(com->tokens);
  free(com);
}

void comment_insert_new_line(comment *com, token comment_token) {
  if (com->line_count == 0) {
    com->start = comment_token.range.start;
  }

  if (com->line_count == com->line_size) {
    com->line_size += 10;

    if (com->tokens) {
      token *p = com->tokens;
      com->tokens = calloc(com->line_size, sizeof(token));
      memcpy(com->tokens, p, sizeof(token) * com->line_count);
      free(p);
    } else {
      com->tokens = calloc(com->line_size, sizeof(token));
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

lexstate *alloc_lexer(rbs_string_t string, const rbs_encoding_t *encoding, int start_pos, int end_pos) {
  lexstate *lexer = malloc(sizeof(lexstate));

  position start_position = (position) {
    .byte_pos = 0,
    .char_pos = 0,
    .line = 1,
    .column = 0,
  };

  *lexer = (lexstate) {
    .string = string,
    .encoding = encoding,
    .start_pos = start_pos,
    .end_pos = end_pos,
    .current = start_position,
    .start = { 0 },
    .first_token_of_line = false,
    .last_char = 0,
  };

  skipn(lexer, start_pos);
  lexer->start = lexer->current;
  lexer->first_token_of_line = lexer->current.column == 0;

  return lexer;
}

parserstate *alloc_parser(lexstate *lexer, int start_pos, int end_pos) {
  parserstate *parser = malloc(sizeof(parserstate));

  *parser = (parserstate) {
    .lexstate = lexer,

    .current_token = NullToken,
    .next_token = NullToken,
    .next_token2 = NullToken,
    .next_token3 = NullToken,

    .vars = NULL,
    .last_comment = NULL,

    .error = NULL,
  };

  parser_advance(parser);
  parser_advance(parser);
  parser_advance(parser);

  return parser;
}

void rbs_parser_declare_type_variables(parserstate *parser, size_t count, const char *variables[count]) {
  if (variables == NULL) return; // Nothing to do.

  parser_push_typevar_table(parser, true);

  for (size_t i = 0; i < count; i++) {
    rbs_constant_id_t name = rbs_constant_pool_insert_literal(fake_constant_pool, variables[i]);
    parser_insert_typevar(parser, name);
  }
}

void free_parser(parserstate *parser) {
  free(parser->lexstate);
  if (parser->last_comment) {
    free_comment(parser->last_comment);
  }
  free(parser);
}
