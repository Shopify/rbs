/**
 *  @file rbs_allocator.c
 *
 *  A simple arena allocator that can be freed all at once.
 *
 *  This allocator maintains a linked list of pages, which come in two flavours:
 *      1. Small allocation pages, which are the same size as the system page size.
 *      2. Large allocation pages, which are the exact size requested, for sizes greater than the small page size.
 *
 *  Small allocations always fit into the unused space at the end of the "head" page. If there isn't enough room, a new
 *  page is allocated, and the small allocation is placed at its start. This approach wastes that unused slack at the
 *  end of the previous page, but it means that allocations are instant and never scan the linked list to find a gap.
 *
 *  This allocator doesn't support freeing individual allocations. Only the whole arena can be freed at once at the end.
 */

#include "rbs/util/rbs_allocator.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h> // for memset()
#include <stdio.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/mman.h>
#endif

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

static void* map_memory(size_t size) {
#ifdef _WIN32
    LPVOID result = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (result == NULL) {
      fprintf(stderr, "VirtualAlloc failed\n");
      abort();
    }
#else
    void* result = mmap(NULL, size, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (result == MAP_FAILED) {
      fprintf(stderr, "mmap failed\n");
      abort();
    }
#endif
    return result;
}

static void destroy_memory(void* memory, size_t size) {
#ifdef _WIN32
  VirtualFree(memory, 0, MEM_RELEASE);
#else
  munmap(memory, size);
#endif
}

static void guard_page(void* memory, size_t page_size) {
#ifdef _WIN32
    DWORD old_protect_;
    BOOL result = VirtualProtect(memory, page_size, PAGE_NOACCESS, &old_protect_);
    if (result == 0) {
      fprintf(stderr, "VirtualProtect failed\n");
      abort();
    }
#else
    int result = mprotect(memory, page_size, PROT_NONE);
    if (result != 0) {
      fprintf(stderr, "mprotect failed\n");
      abort();
    }
#endif
}

// Align `val' to nearest multiple of `alignment'.
static uintptr_t align(uintptr_t val, uintptr_t alignment) {
  return (val + alignment - 1) & ~(alignment - 1);
}

size_t rbs_allocator_default_mem(void) {
    size_t kib = 1024;
    size_t mib = kib * 1024;
    size_t gib = mib * 1024;
    size_t tib = gib * 1024;
    return 4 * tib;
}

void rbs_allocator_init(rbs_allocator_t *allocator, size_t size) {
    size_t page_size = get_system_page_size();
    size = align(size, page_size);
#define is_page_aligned(val) ((val) == align((val), page_size))
    assert(is_page_aligned(size));
    void* mem = map_memory(size + page_size);
    // Guard page; remove range checks in alloc fast path and hard fail if we
    // consume all memory
    void* last_page = (char *) mem + size;
    guard_page(last_page, page_size);
    uintptr_t start = (uintptr_t) mem;
    uintptr_t end = start + size;
    assert(is_page_aligned(end));
    *allocator = (rbs_allocator_t) {
      .start = start,
      .heap_ptr = start,
      .end = end,
    };
}

void rbs_allocator_free(rbs_allocator_t *allocator) {
  if (allocator->start == 0) { return; }
    destroy_memory((void *) allocator->start, allocator->end - allocator->start);
    *allocator = (rbs_allocator_t) {
      .start = 0,
      .heap_ptr = 0,
      .end = 0,
    };
}

// Allocates `size` bytes from `allocator`, aligned to an `alignment`-byte boundary.
void *rbs_allocator_malloc_impl(rbs_allocator_t *allocator, size_t size, size_t alignment) {
    assert(size % alignment == 0 && "size must be a multiple of the alignment");
    uintptr_t aligned = align(allocator->heap_ptr, alignment);
    allocator->heap_ptr = aligned + size;
    return (void *) aligned;
}

// Note: This will eagerly fill with zeroes, unlike `calloc()` which can map a page in a page to be zeroed lazily.
//       It's assumed that callers to this function will immediately write to the allocated memory, anyway.
void *rbs_allocator_calloc_impl(rbs_allocator_t *allocator, size_t count, size_t size, size_t alignment) {
    void *result = rbs_allocator_malloc_impl(allocator, count * size, alignment);
    // Mapping with MAP_ANONYMOUS guarantees that the pages will be already zeroed. For Windows, though,
    // we need to explicitly zero.
#ifdef _WIN32
    memset(result, 0, size);
#endif
    return result;
}
