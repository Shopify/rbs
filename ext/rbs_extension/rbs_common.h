#ifndef RBS__RBS_COMMON_H
#define RBS__RBS_COMMON_H

// Backport `noreturn` until we're on C11.
#define noreturn __attribute__((noreturn))

noreturn void rbs_fatal_error(const char *fmt, ...);

#endif
