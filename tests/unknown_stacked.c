#include <assert.h>
#include <unistd.h>
#include "args.h"

static void success(void) { _exit(EXIT_SUCCESS); }

//< -bcAd
//> ERROR: Unknown or invalid option 'A' in "-bcAd".
int main(int argc, char **argv) {
    atexit(success);

    Args a = {0};
    (void) !option_flag(&a, "optb", NULL, .short_name = 'b');
    (void) !option_flag(&a, "optc", NULL, .short_name = 'c');
    (void) !option_flag(&a, "optd", NULL, .short_name = 'd');

    parse_args(&a, argc, argv, NULL);

    free_args(&a);
    _exit(EXIT_FAILURE);
}
