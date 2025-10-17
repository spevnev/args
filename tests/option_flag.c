#include <assert.h>
#include "args.h"

//< --long1 -s
int main(int argc, char **argv) {
    args a = {0};
    bool *l1 = option_flag(&a, '\0', "long1", NULL);
    bool *s = option_flag(&a, 's', "long2", NULL);
    bool *d = option_flag(&a, '\0', "long3", NULL);

    char **pos_args;
    int pos_args_len = parse_args(&a, argc, argv, &pos_args);
    assert(pos_args_len == 0);

    assert(*l1);
    assert(*s);
    assert(!*d);

    free_args(&a);
    return EXIT_SUCCESS;
}
