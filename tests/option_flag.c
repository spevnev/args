#include <assert.h>
#include "args.h"

//< --long1 -s
int main(int argc, char **argv) {
    Args a = {0};
    const bool *l = option_flag(&a, "long1", NULL);
    const bool *s = option_flag(&a, "long2", NULL, .short_name = 's');
    const bool *d = option_flag(&a, "long3", NULL);

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    assert(*l);
    assert(*s);
    assert(!*d);

    free_args(&a);
    return EXIT_SUCCESS;
}
