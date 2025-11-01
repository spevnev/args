#include <assert.h>
#include <unistd.h>
#include "args.h"

static void success(void) { _exit(EXIT_SUCCESS); }

//< -o1
//> ERROR: Short option must be separate: "-o1".
int main(int argc, char **argv) {
    atexit(success);

    Args a = {0};
    (void) !option_long(&a, 'o', "opt", NULL, false, 0);

    parse_args(&a, argc, argv, NULL);
    _exit(EXIT_FAILURE);
}
