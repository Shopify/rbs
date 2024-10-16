#ifndef RBS_LOCATION_H
#define RBS_LOCATION_H

#include "ruby.h"
#include "lexer_helpers.h"
#include "rbs_location_helpers.h"

/**
 * RBS::Location class
 * */
extern VALUE RBS_Location;

typedef struct {
  VALUE buffer;
  rbs_loc_range rg;
  rbs_loc_children *children; // NULL when no children is allocated
} rbs_loc;

/**
 * Returns new RBS::Location object, with given buffer and range.
 * */
VALUE rbs_new_location(VALUE buffer, range rg);

/**
 * Returns new RBS::Location object, with given buffer and rbs_loc_range.
 * */
VALUE rbs_new_location_from_loc_range(VALUE buffer, rbs_loc_range rg);

/**
 * Return rbs_loc associated with the RBS::Location object.
 * */
rbs_loc *rbs_check_location(VALUE location);

/**
 * Allocate memory for child locations.
 *
 * Do not call twice for the same location.
 * */
void rbs_loc_alloc_children(rbs_loc *loc, unsigned short cap);

/**
 * Add a required child range with given name.
 *
 * Allocate memory for children with rbs_loc_alloc_children before calling this function.
 * */
void rbs_loc_add_required_child(rbs_loc *loc, ID name, range r);

/**
 * Add an optional child range with given name.
 *
 * Allocate memory for children with rbs_loc_alloc_children before calling this function.
 * */
void rbs_loc_add_optional_child(rbs_loc *loc, ID name, range r);

/**
 * Returns RBS::Location object with start/end positions.
 *
 * @param start_pos
 * @param end_pos
 * @return New RSS::Location object.
 * */
VALUE rbs_location_pp(VALUE buffer, const position *start_pos, const position *end_pos);

/**
 * Define RBS::Location class.
 * */
void rbs__init_location();

#endif
