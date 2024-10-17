#include "rbs_buffer.h"

const rbs_buffer_t RBS_BUFFER_NULL = {
    .name = (rbs_string_t) {.start = NULL, .end = NULL, .cached_ruby_string = Qnil},
    .content = (rbs_string_t) {.start = NULL, .end = NULL, .cached_ruby_string = Qnil}
};

rbs_buffer_t rbs_buffer_new(const rbs_string_t name, const rbs_string_t content) {
    return (const rbs_buffer_t) {.name = name, .content = content};
}
