#pragma once

struct program_options {
    bool trust_all = false;
};

void parse_argv(int argc, char **argv);
const program_options& get_program_options();
