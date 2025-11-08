#include <assert.h>
#include "args.h"

//< --long1 FIRST --long2=second -s third
int main(int argc, char **argv) {
    const char *ENUM_VALUES[] = {"first", "second", "third", NULL};

    Args a = {0};
    const size_t *l1 = option_enum(&a, "long1", NULL, ENUM_VALUES, .required = true);
    const size_t *l2 = option_enum(&a, "long2", NULL, ENUM_VALUES, .required = true);
    const size_t *s = option_enum(&a, "long3", NULL, ENUM_VALUES, .short_name = 's', .required = true);
    const size_t *d = option_enum(&a, "long4", NULL, ENUM_VALUES, .default_value = 100);

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    assert(*l1 == 0);
    assert(*l2 == 1);
    assert(*s == 2);
    assert(*d == 100);

    free_args(&a);
    return EXIT_SUCCESS;
}
