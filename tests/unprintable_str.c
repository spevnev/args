#include <assert.h>
#include "args.h"

//> Options:
//>       --str  An option with unprintable default (default: "default \n\t\r\x07\xff")
int main(int argc, char **argv) {
    Args a = {0};
    (void) !option_str(&a, '\0', "str", "An option with unprintable default", true, "default \n\t\r\a\xFF");

    parse_args(&a, argc, argv, NULL);
    print_options(&a, stdout);

    free_args(&a);
    return EXIT_SUCCESS;
}
