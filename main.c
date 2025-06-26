#include <string.h>
#include <stdio.h>
#include <time.h>
#include "rbs.h"
#include "rbs/util/rbs_encoding.h"

#define MAX_LINE_LENGTH 1024
#define DEBUG_PRINT false

void parse_lines_in(const char *file_path, bool print_results) {
    // Open RBS file
    FILE *file = fopen(file_path, "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", file_path);
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    int line_count = 0;
    int successful_parses = 0;
    int failed_parses = 0;

    // Start timing
    clock_t start_time = clock();

    if (print_results) {
        printf("Parsing %s line by line...\n", file_path);
    }

    rbs_allocator_t *allocator = rbs_allocator_init();

    // Read and parse each line
    while (fgets(line, sizeof(line), file)) {
        line_count++;

        // Remove newline character
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
            len--;
        }

        // Skip empty lines
        if (len == 0) {
            continue;
        }

        // Skip and process comment-style type annotations (#:)
        char *parse_line = line;
        if (strncmp(line, "#: ", 3) == 0) {
            // Skip "#:" and any following whitespace
            parse_line = line + 3;
            while (*parse_line == ' ' || *parse_line == '\t') {
                parse_line++;
            }
            len = strlen(parse_line);
            // Skip if line becomes empty after stripping
            if (len == 0) {
                continue;
            }
        }

        // Create RBS string for this line
        rbs_string_t rbs_string = {
            .start = parse_line,
            .end = parse_line + len
        };

        // Create parser for this line
        rbs_parser_t *parser = rbs_parser_new_with_allocator(rbs_string, RBS_ENCODING_UTF_8_ENTRY, 0, (int) len, allocator);
        if (!parser) {
            printf("Error: Failed to create RBS parser for line %d\n", line_count);
            failed_parses++;
            continue;
        }

        // Try to parse method type
        rbs_method_type_t *method_type = NULL;
        bool success = rbs_parse_method_type(parser, &method_type);

        if (success && method_type) {
            successful_parses++;
            if (DEBUG_PRINT) printf("✓ Line %d: Successfully parsed\n", line_count);
        } else {
            failed_parses++;
            if (DEBUG_PRINT) printf("✗ Line %d: Failed to parse: '%s'\n", line_count, parse_line);
        }

        // Cleanup parser for this line
        rbs_parser_free(parser);
    }

    rbs_allocator_free(allocator);

    // End timing
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    fclose(file);

    // Print results
    if (print_results) {
        printf("\n=== Parsing Results ===\n");
        printf("Total lines processed: %d\n", line_count);
        printf("Successful parses: %d\n", successful_parses);
        printf("Failed parses: %d\n", failed_parses);
        printf("Success rate: %.2f%%\n", (double)successful_parses / line_count * 100.0);
        printf("Total time: %.4f seconds\n", elapsed_time);
        printf("Average time per line: %.6f seconds\n", elapsed_time / line_count);
    }
}

int main(int argc, const char * argv[]) {
    const size_t num_uniquely_interned_strings = 26;
    rbs_constant_pool_init(RBS_GLOBAL_CONSTANT_POOL, num_uniquely_interned_strings);
#ifdef rbs__init_arena_allocator
    rbs__init_arena_allocator();
#endif
    
    if (argc < 2) {
        printf("Usage: %s <rbs_file>\n", argv[0]);
        return 1;
    }
    
    const int runs = 10;
    
    // Test parse, to see the stats
    parse_lines_in(argv[1], true);
    
    clock_t start_time = clock();
    for (int i = 0; i < runs; i++) {
        parse_lines_in(argv[1], false);
    }
    clock_t end_time = clock();
    
    double elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Total time for %i runs: %.4f seconds\n", runs, elapsed_time);
    printf("Average time per run: %.6f seconds\n", elapsed_time / runs);
    
    return 0;
}
