#include "rbs_buffer.h"

rbs_buffer_t rbs_buffer_new(const rbs_string_t name, const rbs_string_t content) {
    return (const rbs_buffer_t) {.name = name, .content = content};
}
