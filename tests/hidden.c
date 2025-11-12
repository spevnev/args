#include <assert.h>
#include "args.h"

//< --hidden-bool --hidden-string="test"
//> Options:
//>   --string  A string option (default: none)
int main(int argc, char **argv) {
    Args a = {0};
    (void) !option_flag(&a, "hidden-bool", NULL, .hidden = true);
    (void) !option_string(&a, "hidden-string", NULL, .hidden = true);
    (void) !option_string(&a, "string", "A string option");

    parse_args(&a, argc, argv, NULL);
    print_options(&a, stdout);

    free_args(&a);
    return EXIT_SUCCESS;
}
