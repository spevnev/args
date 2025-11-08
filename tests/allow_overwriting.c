#define ARGS_ALLOW_OVERWRITING
#include <assert.h>
#include "args.h"

//< --opt 1 -o 2 --opt 3
int main(int argc, char **argv) {
    Args a = {0};
    const long *opt = option_long(&a, "opt", NULL, .short_name = 'o', .required = true);

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    assert(*opt == 3);

    free_args(&a);
    return EXIT_SUCCESS;
}
