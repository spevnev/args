#include <assert.h>
#include "args.h"

//< --long1 1 --long2=0x2 -s 03
int main(int argc, char **argv) {
    Args a = {0};
    long *l1 = option_long(&a, '\0', "long1", NULL, false, 0);
    long *l2 = option_long(&a, '\0', "long2", NULL, false, 0);
    long *s = option_long(&a, 's', "long3", NULL, false, 0);
    long *d = option_long(&a, '\0', "long4", NULL, true, 4);

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    assert(*l1 == 1);
    assert(*l2 == 2);
    assert(*s == 3);
    assert(*d == 4);

    free_args(&a);
    return EXIT_SUCCESS;
}
