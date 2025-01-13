#ifndef RBS__PARSERSTATE_H
#define RBS__PARSERSTATE_H

#include "defines.h"
#include "ast.h"
#include "lexer.h"

#include <stdbool.h>
#include <stddef.h>

/**
 * id_table represents a set of RBS constant IDs.
 * This is used to manage the set of bound variables.
 * */
typedef struct id_table {
  size_t size;
  size_t count;
  rbs_constant_id_t *ids;
  struct id_table *next;
} id_table;

/**
 * comment represents a sequence of comment lines.
 *
 *     # Comment for the method.
 *     #
 *     # ```rb
 *     # object.foo()  # Do something
 *     # ```
 *     #
 *     def foo: () -> void
 *
 * A comment object represents the six lines of comments.
 * */
typedef struct comment {
  position start;
  position end;

  size_t line_size;
  size_t line_count;
  token *tokens;

  struct comment *next_comment;
} comment;

typedef struct error {
  char *message;
  token token;
  bool syntax_error;
} error;

/**
 * An RBS parser is a LL(3) parser.
 * */
typedef struct {
  lexstate *lexstate;

  token current_token;
  token next_token;       /* The first lookahead token */
  token next_token2;      /* The second lookahead token */
  token next_token3;      /* The third lookahead token */

  id_table *vars;         /* Known type variables */
  comment *last_comment;  /* Last read comment */

  rbs_constant_pool_t constant_pool;
  error *error;

#ifdef RBS_MAC_OS_USE_SIGNPOSTS
  os_signpost_id_t signpost_id;
#endif
} parserstate;

comment *alloc_comment(token comment_token, comment *last_comment);
void free_comment(comment *com);
void comment_insert_new_line(comment *com, token comment_token);
comment *comment_get_comment(comment *com, int line);

/**
 * Insert new table entry.
 * Setting `reset` inserts a _reset_ entry, which stops searching.
 *
 * ```
 * class Foo[A]
 *          ^^^                      <= push new table with reset
 *   def foo: [B] () -> [A, B]
 *            ^^^                    <= push new table without reset
 *
 *   class Baz[C]
 *            ^^^                    <= push new table with reset
 *   end
 * end
 * ```
 * */
id_table *parser_push_typevar_table(parserstate *state, bool reset);
NODISCARD bool parser_pop_typevar_table(parserstate *state);

/**
 * Insert new type variable into the latest table.
 * */
NODISCARD bool parser_insert_typevar(parserstate *state, rbs_constant_id_t id);

/**
 * Returns true if given type variable is recorded in the table.
 * If not found, it goes one table up, if it's not a reset table.
 * Or returns false, if it's a reset table.
 * */
bool parser_typevar_member(parserstate *state, rbs_constant_id_t id);

/**
 * Allocate new lexstate object.
 *
 * ```
 * VALUE string = rb_funcall(buffer, rb_intern("content"), 0);
 * alloc_lexer(string, 0, 31)    // New lexstate with buffer content
 * ```
 * */
lexstate *alloc_lexer(rbs_string_t string, const rbs_encoding_t *encoding, int start_pos, int end_pos);

/**
 * Allocate new parserstate object.
 *
 * Once allocated, optionally call `rbs_parser_declare_type_variables` if you'd like to declare
 * any type variables to be used during the parsing.
 *
 * ```
 * alloc_parser(buffer, lexer, 0, 1)
 * alloc_parser(buffer, lexer, 3, 5)
 * ```
 * */
parserstate *alloc_parser(lexstate *lexer, int start_pos, int end_pos);
void free_parser(parserstate *parser);

/**
 * Advance one token.
 * */
void parser_advance(parserstate *state);

/**
 * Advance one token if the next_token is a token of the type.
 * */
bool parser_advance_if(parserstate *state, enum TokenType type);
void print_parser(parserstate *state);

/**
 * Insert new comment line token.
 * */
void insert_comment_line(parserstate *state, token token);

/**
 * Returns a RBS::Comment object associated with an subject at `subject_line`.
 *
 * ```rbs
 * # Comment1
 * class Foo           # This is the subject line for Comment1
 *
 *   # Comment2
 *   %a{annotation}    # This is the subject line for Comment2
 *   def foo: () -> void
 * end
 * ```
 * */
rbs_ast_comment_t *get_comment(parserstate *state, int subject_line);

#endif
