#include <assert.h>
#include "args.h"

//> Options:
//>   --str        (default: "default \n\t\r\x07\xff")
//>   --enum-str   (default: "default \n\t\r\x07\xff")
int main(int argc, char **argv) {
    Args a = {0};
    (void) !option_string(&a, '\0', "str", NULL, true, "default \n\t\r\a\xFF");
    (void) !option_enum_string(&a, '\0', "enum-str", NULL, (const char *[]) {NULL}, true, "default \n\t\r\a\xFF");

    parse_args(&a, argc, argv, NULL);
    print_options(&a, stdout);

    free_args(&a);
    return EXIT_SUCCESS;
}
