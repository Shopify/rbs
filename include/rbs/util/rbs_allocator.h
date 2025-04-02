#ifndef RBS_ALLOCATOR_H
#define RBS_ALLOCATOR_H

#include <stddef.h>
#include <inttypes.h>

typedef struct rbs_allocator {
    uintptr_t start;
    uintptr_t end;
    uintptr_t heap_ptr;
} rbs_allocator_t;

size_t rbs_allocator_default_mem(void);
void rbs_allocator_init(rbs_allocator_t *, size_t size);
void rbs_allocator_free(rbs_allocator_t *);
void *rbs_allocator_malloc_impl(rbs_allocator_t *, /*    1    */ size_t size, size_t alignment);
void *rbs_allocator_calloc_impl(rbs_allocator_t *, size_t count, size_t size, size_t alignment);

#define rbs_allocator_alloc(allocator, type)         ((type *) rbs_allocator_malloc_impl((allocator),          sizeof(type), alignof(type)))
#define rbs_allocator_calloc(allocator, count, type) ((type *) rbs_allocator_calloc_impl((allocator), (count), sizeof(type), alignof(type)))

#endif
