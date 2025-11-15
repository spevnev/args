#include "args.h"

// When there's only one matching option, there shouldn't be a description.
//< __complete bash '' '--o' '2'
//> --option
int main(int argc, char **argv) {
    Args a = {0};
    (void) !option_flag(&a, "option", "Description");
    (void) !option_long(&a, "non-matching-option", "Description");

    parse_args(&a, argc, argv, NULL);
    return EXIT_FAILURE;
}
