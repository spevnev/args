#include <assert.h>
#include "args.h"

//< --ignore-required
int main(int argc, char **argv) {
    Args a = {0};
    (void) !option_flag(&a, "ignore-required", NULL, .ignore_required = true);
    (void) !option_long(&a, "opt", NULL, .required = true);

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    free_args(&a);
    return EXIT_SUCCESS;
}
