#include <assert.h>
#include "args.h"

//< --long1 first --long2=second -s third
int main(int argc, char **argv) {
    Args a = {0};
    const char **l1 = option_path(&a, "long1", NULL, .required = true);
    const char **l2 = option_path(&a, "long2", NULL, .required = true);
    const char **s = option_path(&a, "long3", NULL, .short_name = 's', .required = true);
    const char **d = option_path(&a, "long4", NULL, .default_value = "fourth");

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    assert(strcmp(*l1, "first") == 0);
    assert(strcmp(*l2, "second") == 0);
    assert(strcmp(*s, "third") == 0);
    assert(strcmp(*d, "fourth") == 0);

    free_args(&a);
    return EXIT_SUCCESS;
}
