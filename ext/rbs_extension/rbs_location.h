#ifndef RBS__RBS_LOCATION_H
#define RBS__RBS_LOCATION_H

#include "rbs_buffer.h"
#include "rbs_location_helpers.h"
#include "lexer_helpers.h"

typedef struct {
  rbs_buffer_t buffer;
  rbs_loc_range range;
  rbs_loc_children *children; // NULL when no children is allocated
} rbs_location_t;

// Constructors
rbs_location_t rbs_location_new(const rbs_buffer_t, const rbs_loc_range);
void rbs_location_init(rbs_location_t *self, const rbs_buffer_t, const rbs_loc_range);
rbs_location_t rbs_location_new_from_lexer_range(const rbs_buffer_t, const range);
rbs_location_t rbs_location_pp2(const rbs_buffer_t, const position *start, const position *end);

// Methods
void rbs_location_alloc_children(rbs_location_t *self, const unsigned short capacity);
void rbs_location_add_required_child(rbs_location_t *self, const MyID name, const range r);
void rbs_location_add_optional_child(rbs_location_t *self, const MyID name, const range r);

#endif
