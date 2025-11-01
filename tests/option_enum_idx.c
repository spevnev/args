#include <assert.h>
#include "args.h"

//< --long1 FIRST --long2=second -s third
int main(int argc, char **argv) {
    const char *ENUM_VALUES[] = {"first", "second", "third", NULL};

    Args a = {0};
    size_t *l1 = option_enum(&a, '\0', "long1", NULL, false, 0, ENUM_VALUES);
    size_t *l2 = option_enum(&a, '\0', "long2", NULL, false, 0, ENUM_VALUES);
    size_t *s = option_enum(&a, 's', "long3", NULL, false, 0, ENUM_VALUES);
    size_t *d = option_enum(&a, '\0', "long4", NULL, true, 100, ENUM_VALUES);

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    assert(*l1 == 0);
    assert(*l2 == 1);
    assert(*s == 2);
    assert(*d == 100);

    free_args(&a);
    return EXIT_SUCCESS;
}
