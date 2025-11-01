#include <assert.h>
#include <unistd.h>
#include "args.h"

static void success(void) { _exit(EXIT_SUCCESS); }

//< -ab
//> ERROR: Short option must be separate: "-ab".
int main(int argc, char **argv) {
    atexit(success);

    Args a = {0};
    (void) !option_flag(&a, 'a', "opta", NULL);
    (void) !option_flag(&a, 'b', "optb", NULL);

    parse_args(&a, argc, argv, NULL);

    free_args(&a);
    _exit(EXIT_FAILURE);
}
