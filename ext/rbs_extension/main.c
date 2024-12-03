#include "rbs_extension.h"

#include "ast_translation.h"

NORETURN(void) raise_error(parserstate *state, error *error) {
  if (!error->syntax_error) {
    rb_raise(rb_eRuntimeError, "Unexpected error");
  }

  VALUE location = rbs_new_location(state->buffer, error->token.range);
  VALUE type = rb_str_new_cstr(token_type_str(error->token.type));

  VALUE rb_error = rb_funcall(
    RBS_ParsingError,
    rb_intern("new"),
    3,
    location,
    rb_str_new_cstr(error->message),
    type
  );

  rb_exc_raise(rb_error);
}

struct parse_type_arg {
  parserstate *parser;
  VALUE require_eof;
};

static VALUE ensure_free_parser(VALUE parser) {
  free_parser((parserstate *)parser);
  return Qnil;
}

static VALUE parse_type_try(VALUE a) {
  struct parse_type_arg *arg = (struct parse_type_arg *)a;
  parserstate *parser = arg->parser;

  if (parser->next_token.type == pEOF) {
    return Qnil;
  }

  rbs_node_t *type;
  parse_type(parser, &type);

  if (parser->error) {
    raise_error(parser, parser->error);
  }

  if (RB_TEST(arg->require_eof)) {
    parser_advance_assert(parser, pEOF);
  }

  return rbs_struct_to_ruby_value(parser, type);
}

static lexstate *alloc_lexer_from_buffer(VALUE buffer, int start_pos, int end_pos) {
  VALUE string = rb_funcall(buffer, rb_intern("content"), 0);
  StringValue(string);
  return alloc_lexer(string, start_pos, end_pos);
}

static VALUE rbsparser_parse_type(VALUE self, VALUE buffer, VALUE start_pos, VALUE end_pos, VALUE variables, VALUE require_eof) {
  lexstate *lexer = alloc_lexer_from_buffer(buffer, FIX2INT(start_pos), FIX2INT(end_pos));
  parserstate *parser = alloc_parser(buffer, lexer, FIX2INT(start_pos), FIX2INT(end_pos), variables);
  struct parse_type_arg arg = {
    parser,
    require_eof
  };
  return rb_ensure(parse_type_try, (VALUE)&arg, ensure_free_parser, (VALUE)parser);
}

static VALUE parse_method_type_try(VALUE a) {
  struct parse_type_arg *arg = (struct parse_type_arg *)a;
  parserstate *parser = arg->parser;

  if (parser->next_token.type == pEOF) {
    return Qnil;
  }

  rbs_methodtype_t *method_type = NULL;
  parse_method_type(parser, &method_type);

  if (parser->error) {
    raise_error(parser, parser->error);
  }

  if (RB_TEST(arg->require_eof)) {
    parser_advance_assert(parser, pEOF);
  }

  return rbs_struct_to_ruby_value(parser, (rbs_node_t *) method_type);
}

static VALUE rbsparser_parse_method_type(VALUE self, VALUE buffer, VALUE start_pos, VALUE end_pos, VALUE variables, VALUE require_eof) {
  lexstate *lexer = alloc_lexer_from_buffer(buffer, FIX2INT(start_pos), FIX2INT(end_pos));
  parserstate *parser = alloc_parser(buffer, lexer, FIX2INT(start_pos), FIX2INT(end_pos), variables);
  struct parse_type_arg arg = {
    parser,
    require_eof
  };
  return rb_ensure(parse_method_type_try, (VALUE)&arg, ensure_free_parser, (VALUE)parser);
}

static VALUE parse_signature_try(VALUE a) {
  parserstate *parser = (parserstate *)a;

  rbs_signature_t *signature = NULL;
  parse_signature(parser, &signature);

  if (parser->error) {
    raise_error(parser, parser->error);
  }

  return rbs_struct_to_ruby_value(parser, (rbs_node_t *) signature);
}

static VALUE rbsparser_parse_signature(VALUE self, VALUE buffer, VALUE end_pos) {
  lexstate *lexer = alloc_lexer_from_buffer(buffer, 0, FIX2INT(end_pos));
  parserstate *parser = alloc_parser(buffer, lexer, 0, FIX2INT(end_pos), Qnil);
  return rb_ensure(parse_signature_try, (VALUE)parser, ensure_free_parser, (VALUE)parser);
}

static VALUE rbsparser_lex(VALUE self, VALUE buffer, VALUE end_pos) {
  lexstate *lexer = alloc_lexer_from_buffer(buffer, 0, FIX2INT(end_pos));
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

void Init_rbs_extension(void) {
#ifdef HAVE_RB_EXT_RACTOR_SAFE
  rb_ext_ractor_safe(true);
#endif
  rbs__init_constants();
  rbs__init_location();
  rbs__init_parser();
}
