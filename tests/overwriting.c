#include <assert.h>
#include <unistd.h>
#include "args.h"

static void success(void) { _exit(EXIT_SUCCESS); }

//< --opt 1 -o 2 --opt 3
//> ERROR: Option 'o' is set more than once.
int main(int argc, char **argv) {
    atexit(success);

    Args a = {0};
    (void) !option_long(&a, 'o', "opt", NULL, false, 0);

    parse_args(&a, argc, argv, NULL);

    free_args(&a);
    _exit(EXIT_FAILURE);
}
