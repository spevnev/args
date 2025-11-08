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
    (void) !option_flag(
        &a, "bool", "Option with a description so long that it must be split to the second line", .short_name = 'b'
    );
    (void) !option_long(&a, "option-with-long-name", "A long option", .default_value = 12);
    (void) !option_float(&a, "float", "A float option", .short_name = 'f', .required = true);
    (void) !option_string(&a, "str", "A string option", .default_value = "some default");
    (void) !option_path(&a, "path", "A path option", .short_name = 'p');
    (void) !option_enum(&a, "enum-idx", "An index enum option", (const char *[]) {NULL}, .default_value = -1);
    (void) !option_enum_string(
        &a, "enum-str", "A string enum option", (const char *[]) {NULL}, .short_name = 'e',
        .default_value = "enum default"
    );

    parse_args(&a, argc, argv, NULL);
    print_options(&a, stdout);

    free_args(&a);
    return EXIT_SUCCESS;
}
