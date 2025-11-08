#include <assert.h>
#include "args.h"

//< -f 1
//> Options:
//>   -b, --bool                   Option with a description so long that it must be
//>                                split to the second line
//>       --option-with-long-name  A long option (default: 12)
//>   -f, --float                  A float option
//>       --str                    A string option (default: "some default")
//>   -p, --path                   A path option (default: none)
//>       --enum-idx               An index enum option (default: none)
//>   -e, --enum-str               A string enum option (default: "enum default")
int main(int argc, char **argv) {
    Args a = {0};
    (void) !option_flag(&a, 'b', "bool", "Option with a description so long that it must be split to the second line");
    (void) !option_long(&a, '\0', "option-with-long-name", "A long option", true, 12);
    (void) !option_float(&a, 'f', "float", "A float option", false, 0.0F);
    (void) !option_string(&a, '\0', "str", "A string option", true, "some default");
    (void) !option_path(&a, 'p', "path", "A path option", true, NULL);
    (void) !option_enum(&a, '\0', "enum-idx", "An index enum option", true, -1ULL, (const char *[]) {NULL});
    (void) !option_enum_string(
        &a, 'e', "enum-str", "A string enum option", true, "enum default", (const char *[]) {NULL}
    );

    parse_args(&a, argc, argv, NULL);
    print_options(&a, stdout);

    free_args(&a);
    return EXIT_SUCCESS;
}
