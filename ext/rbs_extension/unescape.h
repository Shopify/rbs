#ifndef RBS_UNESCAPE_H
#define RBS_UNESCAPE_H

#include <stddef.h>
#include "parserstate.h"

/**
 * Receives `parserstate` and `range`, which represents a string token or symbol token, and returns a string VALUE.
 *
 *    Input token | Output string
 *    ------------+-------------
 *    "foo\\n"    | foo\n
 *    'foo'       | foo
 *    `bar`       | bar
 *    :"baz\\t"   | baz\t
 *    :'baz'      | baz
 * */
rbs_string_t rbs_unquote_string(parserstate *state, range rg, int offset_bytes);

#endif // RBS_UNESCAPE_H
