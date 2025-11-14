#include <assert.h>
#include <unistd.h>
#include "args.h"

static void success(void) { _exit(EXIT_SUCCESS); }

//< --opt 1 -o 2 --opt 3
//> ERROR: Option "opt" is already set: "-o".
int main(int argc, char **argv) {
    atexit(success);

    Args a = {0};
    (void) !option_long(&a, "opt", NULL, .short_name = 'o', .required = true);

    parse_args(&a, argc, argv, NULL);

    free_args(&a);
    _exit(EXIT_FAILURE);
}
