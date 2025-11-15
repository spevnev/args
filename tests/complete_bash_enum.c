#include "args.h"

//< __complete bash '--enum' '' '2'
//> first
//> second
//> third
int main(int argc, char **argv) {
    Args a = {0};
    (void) !option_enum(&a, "enum", NULL, ((const char *[]) {"first", "second", "third", NULL}));

    parse_args(&a, argc, argv, NULL);
    return EXIT_FAILURE;
}
