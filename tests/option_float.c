#include <assert.h>
#include "args.h"

//< --long1 0.1 --long2=0.2 -s .3
int main(int argc, char **argv) {
    Args a = {0};
    const float *l1 = option_float(&a, "long1", NULL, .required = true);
    const float *l2 = option_float(&a, "long2", NULL, .required = true);
    const float *s = option_float(&a, "long3", NULL, .short_name = 's', .required = true);
    const float *d = option_float(&a, "long4", NULL, .default_value = 0.4F);

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    assert(*l1 == 0.1F);
    assert(*l2 == 0.2F);
    assert(*s == 0.3F);
    assert(*d == 0.4F);

    free_args(&a);
    return EXIT_SUCCESS;
}
