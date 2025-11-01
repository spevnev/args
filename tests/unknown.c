#include <assert.h>
#include <unistd.h>
#include "args.h"

static void success(void) { _exit(EXIT_SUCCESS); }

//< --unknown
//> ERROR: Unknown or invalid option "unknown".
int main(int argc, char **argv) {
    atexit(success);

    Args a = {0};
    (void) !option_long(&a, 'o', "opt", NULL, true, 0);

    parse_args(&a, argc, argv, NULL);
    _exit(EXIT_FAILURE);
}
