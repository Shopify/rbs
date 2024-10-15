#include <stdbool.h>

#include "ruby.h"
#include "ruby/re.h"
#include "ruby/encoding.h"

#include "lexer.h"
#include "parser.h"
#include "constants.h"
#include "ruby_objs.h"

/**
 * Raises RBS::ParsingError on `tok` with message constructed with given `fmt`.
 *
 * ```
 * foo.rbs:11:21...11:25: Syntax error: {message}, token=`{tok source}` ({tok type})
 * ```
 * */
PRINTF_ARGS(NORETURN(void) raise_syntax_error(parserstate *state, token tok, const char *fmt, ...), 3, 4);
