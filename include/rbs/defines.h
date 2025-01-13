/**
 * @file defines.h
 *
 * Macro definitions used throughout the rbs library.
 *
 * This file should be included first by any *.h or *.c in rbs for consistency
 * and to ensure that the macros are defined before they are used.
 */

#ifndef RBS_DEFINES_H
#define RBS_DEFINES_H


/***********************************************************************************************************************
 * Copied+modified subset of Prism's `include/prism/defines.h`                                                         *
 **********************************************************************************************************************/

/**
 * Certain compilers support specifying that a function accepts variadic
 * parameters that look like printf format strings to provide a better developer
 * experience when someone is using the function. This macro does that in a
 * compiler-agnostic way.
 */
#if defined(__GNUC__)
#   if defined(__MINGW_PRINTF_FORMAT)
#       define RBS_ATTRIBUTE_FORMAT(string_index, argument_index) __attribute__((format(__MINGW_PRINTF_FORMAT, string_index, argument_index)))
#   else
#       define RBS_ATTRIBUTE_FORMAT(string_index, argument_index) __attribute__((format(printf, string_index, argument_index)))
#   endif
#elif defined(__clang__)
#   define RBS_ATTRIBUTE_FORMAT(string_index, argument_index) __attribute__((__format__(__printf__, string_index, argument_index)))
#else
#   define RBS_ATTRIBUTE_FORMAT(string_index, argument_index)
#endif

/**
 * We use -Wimplicit-fallthrough to guard potentially unintended fall-through between cases of a switch.
 * Use RBS_FALLTHROUGH to explicitly annotate cases where the fallthrough is intentional.
 */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L // C23 or later
    #define RBS_FALLTHROUGH [[fallthrough]];
#elif defined(__GNUC__) || defined(__clang__)
    #define RBS_FALLTHROUGH __attribute__((fallthrough));
#elif defined(_MSC_VER)
    #define RBS_FALLTHROUGH __fallthrough;
#else
    #define RBS_FALLTHROUGH
#endif

/***********************************************************************************************************************
 * Custom defines for RBS                                                                                              *
 **********************************************************************************************************************/

#define NODISCARD __attribute__((warn_unused_result))

#undef NDEBUG // enable assertions and other debugging features.

#if !defined(NDEBUG) && defined(__APPLE__)
    #define RBS_MAC_OS_USE_SIGNPOSTS

    #include <os/log.h>
    #include <os/signpost.h>
#else // Define no-op stubs for other platforms.
    typedef void * os_log_t;
    typedef void * os_signpost_id_t;

    #define os_log_create(subsystem, category) NULL
    #define OS_LOG_CATEGORY_POINTS_OF_INTEREST ((const char *) 0) NULL

    #define os_signpost_id_generate(log) NULL
    #define os_signpost_id_make_with_pointer(log, ptr) NULL
    #define os_signpost_event_emit(log, event_id, name, ...) NULL
    #define os_signpost_interval_begin(log, interval_id, name, ...) NULL
    #define os_signpost_interval_end(log, interval_id, name, ...) NULL
#endif

#endif
