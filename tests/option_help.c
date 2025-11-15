#include <assert.h>
#include "args.h"

//< -h
//> help message
static void print_help(Args *args, const char *program_name) {
    (void) args;
    (void) program_name;
    printf("help message\n");
}

int main(int argc, char **argv) {
    Args a = {0};
    option_help(&a, print_help);

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    free_args(&a);
    return EXIT_SUCCESS;
}
