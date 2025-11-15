#include "args.h"

//< __complete zsh
//> --+option-=[ \] \\]:+option-:(\ a\  \) \\)
//> *:file:_files
int main(int argc, char **argv) {
    Args a = {0};
    (void) !option_enum(&a, "+option-", "\n] \\", ((const char *[]) {" a ", ")", "\\", NULL}));

    parse_args(&a, argc, argv, NULL);
    return EXIT_FAILURE;
}
