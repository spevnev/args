#include <assert.h>
#include "args.h"

//< --long1 1 --long2=0x2 -s 03
int main(int argc, char **argv) {
    Args a = {0};
    const long *l1 = option_long(&a, "long1", NULL, .required = true);
    const long *l2 = option_long(&a, "long2", NULL, .required = true);
    const long *s = option_long(&a, "long3", NULL, .short_name = 's', .required = true);
    const long *d = option_long(&a, "long4", NULL, .default_value = 4);

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    assert(*l1 == 1);
    assert(*l2 == 2);
    assert(*s == 3);
    assert(*d == 4);

    free_args(&a);
    return EXIT_SUCCESS;
}
