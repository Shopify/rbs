#ifndef RBS__LEXER_HELPERS_H
#define RBS__LEXER_HELPERS_H

/**
 * The `byte_pos` (or `char_pos`) is the primary data.
 * The rest are cache.
 *
 * They can be computed from `byte_pos` (or `char_pos`), but it needs full scan from the beginning of the string (depending on the encoding).
 * */
typedef struct {
  int byte_pos;
  int char_pos;
  int line;
  int column;
} position;

typedef struct {
  position start;
  position end;
} range;

#endif
