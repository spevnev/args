#define ARGS_SKIP_UNKNOWN
#include <assert.h>
#include <unistd.h>
#include "args.h"

//< --unknown --opt 1
int main(int argc, char **argv) {
    Args a = {0};
    const long *opt = option_long(&a, 'o', "opt", NULL, true, 0);

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    assert(*opt == 1);

    free_args(&a);
    return EXIT_SUCCESS;
}
