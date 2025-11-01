#include <assert.h>
#include "args.h"

//< --long1 first --long2=second -s third
int main(int argc, char **argv) {
    Args a = {0};
    const char **l1 = option_str(&a, '\0', "long1", NULL, false, NULL);
    const char **l2 = option_str(&a, '\0', "long2", NULL, false, NULL);
    const char **s = option_str(&a, 's', "long3", NULL, false, NULL);
    const char **d = option_str(&a, '\0', "long4", NULL, true, "fourth");

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    assert(strcmp(*l1, "first") == 0);
    assert(strcmp(*l2, "second") == 0);
    assert(strcmp(*s, "third") == 0);
    assert(strcmp(*d, "fourth") == 0);

    free_args(&a);
    return EXIT_SUCCESS;
}
