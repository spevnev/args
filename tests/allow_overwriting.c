#define ARGS_ALLOW_OVERWRITING
#include <assert.h>
#include "args.h"

//< --opt 1 -o 2 --opt 3
int main(int argc, char **argv) {
    Args a = {0};
    long *opt = option_long(&a, 'o', "opt", NULL, false, 0);

    char **pos_args;
    int pos_args_len = parse_args(&a, argc, argv, &pos_args);
    assert(pos_args_len == 0);

    assert(*opt == 3);

    free_args(&a);
    return EXIT_SUCCESS;
}
