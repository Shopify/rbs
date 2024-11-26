#ifndef RBS__CONSTANT_POOL_H
#define RBS__CONSTANT_POOL_H

#include <stdint.h>
#include <stddef.h>

// TODO: Use a more compact integer type.
/**
 * A constant id is a unique identifier for a constant in the constant pool.
 */
typedef uintptr_t rbs_constant_id_t;

/** A constant in the pool which effectively stores a string. */
typedef struct {
    /** A pointer to the start of the string. */
    const char *start;

    /** The length of the string. */
    size_t length;
} rbs_constant_t;

typedef struct {
    // TODO: Implement this.
} rbs_constant_pool_t;

// A temporary stand-in for the constant pool until we implement it.
extern rbs_constant_pool_t *fake_constant_pool;

/**
 * Return a pointer to the constant indicated by the given constant id.
 *
 * @param pool The pool to get the constant from.
 * @param constant_id The id of the constant to get.
 * @return A pointer to the constant.
 */
rbs_constant_t *rbs_constant_pool_id_to_constant(const rbs_constant_pool_t *pool, rbs_constant_id_t constant_id);

/**
 * Insert a constant into a constant pool from memory that is constant. Returns
 * the id of the constant, or 0 if any potential calls to resize fail.
 *
 * @param pool The pool to insert the constant into.
 * @param start A pointer to the start of the constant.
 * @param length The length of the constant.
 * @return The id of the constant.
 */
rbs_constant_id_t rbs_constant_pool_insert_constant(rbs_constant_pool_t *pool, const char *start, size_t length);

// Convenience function for `rbs_constant_pool_insert_constant()` which calculates the length internally.
rbs_constant_id_t rbs_constant_pool_insert_literal(rbs_constant_pool_t *pool, const char *start);

#endif
