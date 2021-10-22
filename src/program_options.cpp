#include "program_options.h"

#include <cstring>

program_options opts;

void parse_argv(int argc, char **argv) {
    if (argc == 2) {
        if (std::strcmp(argv[1], "--trust-all") == 0) {
            opts.trust_all = true;
        }
    }
}

const program_options& get_program_options() {
    return opts;
}