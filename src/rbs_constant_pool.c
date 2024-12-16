#include "rbs/rbs_constant_pool.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

rbs_constant_pool_t *fake_constant_pool = (rbs_constant_pool_t *) 0xdeadbeef;

rbs_constant_t *rbs_constant_pool_id_to_constant(const rbs_constant_pool_t *self, rbs_constant_id_t constant_id) {
    // printf("trace: rbs_constant_pool_id_to_constant(%p, %p)\n", self, (void *)constant_id);
    assert(self == fake_constant_pool);

    // Convert the fake ID integer back into a pointer,
    // until we implement real pooling.
    return (rbs_constant_t *) constant_id;
}

rbs_constant_id_t rbs_constant_pool_insert_constant(rbs_constant_pool_t *self, const char *start, size_t length) {
    // printf("trace: rbs_constant_pool_insert_constant(%p, %s, %zu)\n", self, start, length);
    // TODO: implement actual pooling.
    assert(self == fake_constant_pool);

    rbs_constant_t *new_constant = malloc(sizeof(rbs_constant_t));

    char *copied_str = malloc(length + 1);
    memcpy(copied_str, start, length);
    copied_str[length] = '\0';

    *new_constant = (rbs_constant_t) {
        .start = copied_str,
        .length = length,
    };

    // Convert the pointer directly to an integer to use as a fake ID,
    // until we implement real pooling.
    return (rbs_constant_id_t) new_constant;
}

rbs_constant_id_t rbs_constant_pool_insert_literal(rbs_constant_pool_t *self, const char *start) {
    // printf("trace: rbs_constant_pool_insert_literal(%p, %s)\n", self, start);
    assert(self == fake_constant_pool);

    return rbs_constant_pool_insert_constant(self, start, strlen(start));
}

bool rbs_constant_id_equal(rbs_constant_pool_t *self, rbs_constant_id_t lhs, rbs_constant_id_t rhs) {
    assert(self == fake_constant_pool);

    if (lhs == rhs) return true;

    rbs_constant_t *lhs_constant = rbs_constant_pool_id_to_constant(self, lhs);
    rbs_constant_t *rhs_constant = rbs_constant_pool_id_to_constant(self, rhs);

    if (lhs_constant->length != rhs_constant->length) return false;

    return strncmp(lhs_constant->start, rhs_constant->start, lhs_constant->length) == 0;
}

void pm_constant_pool_destroy_constant(rbs_constant_pool_t *pool, rbs_constant_id_t constant_id) {
    assert(pool == fake_constant_pool);

    rbs_constant_t *constant = rbs_constant_pool_id_to_constant(pool, constant_id);
    free((void *) constant->start);
    free(constant);
}
