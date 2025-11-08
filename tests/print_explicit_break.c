#include <assert.h>
#include "args.h"

//> Options:
//>   -o, --opt  Option with a very long description
//>              that is explicitly split into multiple
//>              lines using '\n'.
int main(int argc, char **argv) {
    Args a = {0};
    (void) !option_flag(
        &a, "opt", "Option with a very long description\nthat is explicitly split into multiple\nlines using '\\n'.",
        .short_name = 'o'
    );

    parse_args(&a, argc, argv, NULL);
    print_options(&a, stdout);

    free_args(&a);
    return EXIT_SUCCESS;
}
