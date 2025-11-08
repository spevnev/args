#include <assert.h>
#include "args.h"

//< -cb -dl 2 -L-3 -esstring
int main(int argc, char **argv) {
    Args a = {0};
    const bool *b = option_flag(&a, 'b', "optb", NULL);
    const bool *c = option_flag(&a, 'c', "optc", NULL);
    const bool *d = option_flag(&a, 'd', "optd", NULL);
    const bool *e = option_flag(&a, 'e', "opte", NULL);
    const long *l = option_long(&a, 'l', "optl", NULL, false, 0);
    const long *L = option_long(&a, 'L', "optL", NULL, false, 0);
    const char **s = option_string(&a, 's', "opts", NULL, false, NULL);

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    assert(*b);
    assert(*c);
    assert(*d);
    assert(*e);
    assert(*l == 2);
    assert(*L == -3);
    assert(strcmp(*s, "string") == 0);

    free_args(&a);
    return EXIT_SUCCESS;
}
