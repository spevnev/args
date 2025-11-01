#include <assert.h>
#include "args.h"

//< 1 --opt option 2 3
int main(int argc, char **argv) {
    Args a = {0};
    (void) !option_str(&a, 'o', "opt", NULL, false, NULL);

    char **pos_args;
    int pos_args_len = parse_args(&a, argc, argv, &pos_args);
    assert(pos_args_len == 3);

    assert(strcmp(pos_args[0], "1") == 0);
    assert(strcmp(pos_args[1], "2") == 0);
    assert(strcmp(pos_args[2], "3") == 0);

    free_args(&a);
    return EXIT_SUCCESS;
}
