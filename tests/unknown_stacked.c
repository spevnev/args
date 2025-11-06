#include <assert.h>
#include <unistd.h>
#include "args.h"

static void success(void) { _exit(EXIT_SUCCESS); }

//< -bcAd
//> ERROR: Unknown or invalid option 'A' in "-bcAd".
int main(int argc, char **argv) {
    atexit(success);

    Args a = {0};
    (void) !option_flag(&a, 'b', "optb", NULL);
    (void) !option_flag(&a, 'c', "optc", NULL);
    (void) !option_flag(&a, 'd', "optd", NULL);

    parse_args(&a, argc, argv, NULL);

    free_args(&a);
    _exit(EXIT_FAILURE);
}
