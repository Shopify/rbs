/**
 *  @file rbs_allocator.c
 *
 *  A simple arena allocator that can be freed all at once.
 *
 *  This allocator doesn't support freeing individual allocations. Only the whole arena can be freed at once at the end.
 */

#include "rbs/util/rbs_allocator.h"
#include "rbs/util/rbs_assert.h"

#include <stdlib.h>
#include <string.h> // for memset()
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#endif

struct rbs_allocator {
    uintptr_t heap_ptr;
    uintptr_t end;
};

static size_t get_system_page_size(void) {
#ifdef _WIN32
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwPageSize;
#else
    long sz = sysconf(_SC_PAGESIZE);
    if (sz == -1) return 4096; // Fallback to the common 4KB page size
    return (size_t) sz;
#endif
}

static void destroy_memory(void *memory) {
    free(memory);
}

static size_t rbs_allocator_default_mem(void) {
    size_t kib = 1024;
    size_t mib = kib * 1024;
    return 4 * mib;
}

static inline bool is_power_of_two(uintptr_t value) {
    return value > 0 && (value & (value - 1)) == 0;
}

// Align `val' to nearest multiple of `alignment'.
static uintptr_t align(uintptr_t size, uintptr_t alignment) {
    rbs_assert(is_power_of_two(alignment), "alignment is not a power of two");
    return (size + alignment - 1) & ~(alignment - 1);
}

rbs_allocator_t *rbs_allocator_init(void) {
    size_t size = rbs_allocator_default_mem();
    size_t page_size = get_system_page_size();
    size = align(size, page_size);
    void *mem = malloc(size);
    uintptr_t start = (uintptr_t) mem;
    rbs_allocator_t header = (rbs_allocator_t) {
        .heap_ptr = start + sizeof header,
        .end = start + size,
    };
    memcpy(mem, &header, sizeof header);
    return (rbs_allocator_t *) mem;
}

void rbs_allocator_free(rbs_allocator_t *allocator) {
    destroy_memory((void *) allocator);
}

// Allocates `new_size` bytes from `allocator`, aligned to an `alignment`-byte boundary.
// Copies `old_size` bytes from `ptr` to the new allocation.
// It always reallocates the memory in new space and thus wastes the old space.
void *rbs_allocator_realloc_impl(rbs_allocator_t *allocator, void *ptr, size_t old_size, size_t new_size, size_t alignment) {
    void *p = rbs_allocator_malloc_impl(allocator, new_size, alignment);
    memcpy(p, ptr, old_size);
    return p;
}

// Allocates `size` bytes from `allocator`, aligned to an `alignment`-byte boundary.
void *rbs_allocator_malloc_impl(rbs_allocator_t *allocator, size_t size, size_t alignment) {
    rbs_assert(size % alignment == 0, "size must be a multiple of the alignment. size: %zu, alignment: %zu", size, alignment);
    uintptr_t aligned = align(allocator->heap_ptr, alignment);
    if (aligned + size >= allocator->end) {
        fprintf(stderr, "Out of memory. Please bump the total arena size (currently %zu) in rbs_allocator_default_mem.", rbs_allocator_default_mem());
        abort();
    }
    allocator->heap_ptr = aligned + size;
    return (void *) aligned;
}

// Note: This will eagerly fill with zeroes, unlike `calloc()` which can map a page in a page to be zeroed lazily.
//       It's assumed that callers to this function will immediately write to the allocated memory, anyway.
void *rbs_allocator_calloc_impl(rbs_allocator_t *allocator, size_t count, size_t size, size_t alignment) {
    void *p = rbs_allocator_malloc_many_impl(allocator, count, size, alignment);
    memset(p, 0, count * size);
    return p;
}

// Similar to `rbs_allocator_malloc_impl()`, but allocates `count` instances of `size` bytes, aligned to an `alignment`-byte boundary.
void *rbs_allocator_malloc_many_impl(rbs_allocator_t *allocator, size_t count, size_t size, size_t alignment) {
    return rbs_allocator_malloc_impl(allocator, count * size, alignment);
}
