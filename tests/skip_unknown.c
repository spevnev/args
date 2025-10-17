#define ARGS_SKIP_UNKNOWN
#include <assert.h>
#include <unistd.h>
#include "args.h"

//< --unknown --opt 1
int main(int argc, char **argv) {
    args a = {0};
    long *opt = option_long(&a, 'o', "opt", NULL, true, 0);

    char **pos_args;
    int pos_args_len = parse_args(&a, argc, argv, &pos_args);
    assert(pos_args_len == 0);

    assert(*opt == 1);

    free_args(&a);
    return EXIT_SUCCESS;
}
