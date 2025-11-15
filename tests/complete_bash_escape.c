#include "args.h"

//< __complete bash '' '-' '2'
//> --+option- --  --  \\"
//> --other
int main(int argc, char **argv) {
    Args a = {0};
    (void) !option_flag(&a, "+option-", " -- \n\\\"");
    // Second option so that description are printed.
    (void) !option_flag(&a, "other", NULL);

    parse_args(&a, argc, argv, NULL);
    return EXIT_FAILURE;
}
