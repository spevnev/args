#include "args.h"

//< __complete bash '' '-' '2'
//> -b, --bool     -- A bool option
//>     --long     -- A long option
//> -f, --float    -- A float option
//>     --string   -- A string option
//> -p, --path     -- A path option
//>     --enum-idx -- An index enum option
//> -e, --enum-str -- A string enum option
int main(int argc, char **argv) {
    const char *ENUM_VALUES[] = {"first", "second", "third", NULL};

    Args a = {0};
    (void) !option_flag(&a, "bool", "A bool option", .short_name = 'b');
    (void) !option_long(&a, "long", "A long option");
    (void) !option_float(&a, "float", "A float option", .short_name = 'f');
    (void) !option_string(&a, "string", "A string option");
    (void) !option_path(&a, "path", "A path option", .short_name = 'p');
    (void) !option_enum(&a, "enum-idx", "An index enum option", ENUM_VALUES);
    (void) !option_enum_string(&a, "enum-str", "A string enum option", ENUM_VALUES, .short_name = 'e');

    parse_args(&a, argc, argv, NULL);
    return EXIT_FAILURE;
}
