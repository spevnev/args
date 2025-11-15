#include <assert.h>
#include "args.h"

// Provide invalid/unknown options to test that early_exit skips validation.
//< --unknown --skip --long "string"
int main(int argc, char **argv) {
    Args a = {0};
    const bool *skip = option_flag(&a, "skip", NULL, .early_exit = true);
    (void) !option_long(&a, "opt", NULL, .required = true);
    (void) !option_long(&a, "long", NULL);

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    assert(*skip);

    free_args(&a);
    return EXIT_SUCCESS;
}
