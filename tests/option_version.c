#include <assert.h>
#include "args.h"

//< -v
//> current version: 1.2.3
int main(int argc, char **argv) {
    Args a = {0};
    option_version(&a, "current version: 1.2.3");

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    free_args(&a);
    return EXIT_SUCCESS;
}
