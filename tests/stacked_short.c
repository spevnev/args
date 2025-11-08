#include <assert.h>
#include "args.h"

//< -cb -dl 2 -L-3 -esstring
int main(int argc, char **argv) {
    Args a = {0};
    const bool *b = option_flag(&a, "optb", NULL, .short_name = 'b');
    const bool *c = option_flag(&a, "optc", NULL, .short_name = 'c');
    const bool *d = option_flag(&a, "optd", NULL, .short_name = 'd');
    const bool *e = option_flag(&a, "opte", NULL, .short_name = 'e');
    const long *l = option_long(&a, "optl", NULL, .short_name = 'l', .required = true);
    const long *L = option_long(&a, "optL", NULL, .short_name = 'L', .required = true);
    const char **s = option_string(&a, "opts", NULL, .short_name = 's', .required = true);

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
