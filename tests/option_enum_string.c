#include <assert.h>
#include "args.h"

//< --long1 FIRST --long2=second -s third
int main(int argc, char **argv) {
    const char *ENUM_VALUES[] = {"first", "second", "third", NULL};

    Args a = {0};
    const char **l1 = option_enum_string(&a, '\0', "long1", NULL, ENUM_VALUES, false, NULL);
    const char **l2 = option_enum_string(&a, '\0', "long2", NULL, ENUM_VALUES, false, NULL);
    const char **s = option_enum_string(&a, 's', "long3", NULL, ENUM_VALUES, false, NULL);
    const char **d = option_enum_string(&a, '\0', "long4", NULL, ENUM_VALUES, true, "DEFAULT");

    int pos_args_len = parse_args(&a, argc, argv, NULL);
    assert(pos_args_len == 0);

    assert(strcmp(*l1, "first") == 0);
    assert(strcmp(*l2, "second") == 0);
    assert(strcmp(*s, "third") == 0);
    assert(strcmp(*d, "DEFAULT") == 0);

    free_args(&a);
    return EXIT_SUCCESS;
}
