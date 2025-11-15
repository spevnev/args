#include <assert.h>
#include "args.h"

// Test that version is printed even if there are invalid/unknown options.
//< --unknown -vabc --other=2
//> current version: 1.2.3
int main(int argc, char **argv) {
    Args a = {0};
    option_version(&a, "current version: 1.2.3");

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    free_args(&a);
    return EXIT_SUCCESS;
}
