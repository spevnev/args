#include <unistd.h>
#include "args.h"

static void success() { _exit(EXIT_SUCCESS); }

//> ERROR: Option values cannot be accessed before `parse_args()`.
int main() {
    atexit(success);

    ArgsCpp args;
    args.option_long("long", nullptr).value();

    return EXIT_FAILURE;
}
