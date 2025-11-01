#include <assert.h>
#include "args.h"

//< --long1 0.1 --long2=0.2 -s .3
int main(int argc, char **argv) {
    Args a = {0};
    float *l1 = option_float(&a, '\0', "long1", NULL, false, 0);
    float *l2 = option_float(&a, '\0', "long2", NULL, false, 0);
    float *s = option_float(&a, 's', "long3", NULL, false, 0);
    float *d = option_float(&a, '\0', "long4", NULL, true, 0.4F);

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    assert(*l1 == 0.1F);
    assert(*l2 == 0.2F);
    assert(*s == 0.3F);
    assert(*d == 0.4F);

    free_args(&a);
    return EXIT_SUCCESS;
}
