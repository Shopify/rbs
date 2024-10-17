#include "rbs_string.h"

const rbs_string_t RBS_STRING_NULL = {.start = NULL, .end = NULL, .cached_ruby_string = Qnil};

rbs_string_t rbs_string_new(const char* start, const char* end) {
    return (const rbs_string_t) {.start = start, .end = end, .cached_ruby_string = Qnil};
}

rbs_string_t rbs_string_offset(const rbs_string_t self, const int offset) {
    return rbs_string_new(self.start + offset, self.end);
}

size_t rbs_string_len(const rbs_string_t self) {
    return self.end - self.start;
}
