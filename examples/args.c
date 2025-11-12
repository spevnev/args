#include "args.h"

int main(int argc, char **argv) {
    // Zero-initialize library state.
    Args a = {0};

    // Define options.
    // All options take: args*, long name, description, and zero or more named arguments.
    // Named arguments are set through designated initializer: `.name1 = value1, .name2 = value2`,
    // with default value 0 / '\0' / false / NULL depending on the type.
    // Named arguments: short_name, required, hidden, default_value.
    const bool *help = option_flag(&a, "help", "Show help", .short_name = 'h', .ignore_required = true);
    const bool *version = option_flag(&a, "version", "Print version", .short_name = 'v', .ignore_required = true);
    const bool *hidden = option_flag(&a, "hidden", NULL, .hidden = true);
    const long *long_ = option_long(&a, "long", "A long option", .short_name = 'l');
    const float *float_ = option_float(&a, "float", "A float option", .short_name = 'f', .required = true);
    const char **string = option_string(
        &a,
        "str",
        "A string option",
        .short_name = 's',
        .default_value = "string default"
    );
    const char **path = option_path(&a, "path", "A path option");

    // If enum is continuous and array matches it, result of `option_enum` can be converted directly.
    typedef enum { FIRST, SECOND, THIRD } Enum;
    const char *enum_values[] = {"first", "second", "third", NULL};
    const Enum *enum_ = (const Enum *)
        option_enum(&a, "enum", "An enum option", enum_values, .short_name = 'e', .default_value = FIRST);
    // If values don't match, or the enum isn't continuous, it may be desirable to get a string instead.
    // Also, if values array are defined in the arguments, it must be wrapped in parenthesis.
    const char **enum_string = option_enum_string(
        &a,
        "enum-str",
        "A string enum option",
        ((const char *[]) {"other", "enum", "values", NULL}),
        .default_value = "default"
    );

    // Parse arguments. Sets option values and returns positional arguments.
    // Handles shell completion by printing to stdout and exiting.
    // Exits on errors: wrong option value, missing required option, unknown option, etc.
    // Must be called before side effects and printing to stdout.
    char **pos_args;
    int pos_args_len = parse_args(&a, argc, argv, &pos_args);

    // Print help on flag or subcommand.
    if (*help || (pos_args_len == 1 && strcmp(pos_args[0], "help") == 0)) {
        // Print description and usage, then call `print_options`.
        printf("%s - Example of using 'args' library\n", argv[0]);
        printf("\n");
        printf("Usage:\n");
        printf("  %s [options]\n", argv[0]);
        // Shell completions can either be left hidden and generated on install,
        // or be exposed to user by printing it in usage and/or documentation.
        printf("  %s completion <bash|zsh|fish>\n", argv[0]);
        printf("\n");
        print_options(&a, stdout);
        free_args(&a);
        return EXIT_SUCCESS;
    }

    // Similarly, print version on flag or subcommand.
    if (*version || (pos_args_len == 1 && strcmp(pos_args[0], "version") == 0)) {
        printf("v1.3.0\n");
        free_args(&a);
        return EXIT_SUCCESS;
    }

    // Use option values.
    printf(
        "options: h=%s l=%ld f=%f s=\"%s\" p=\"%s\" e=%d E=\"%s\"\n",
        *hidden ? "true" : "false",
        *long_,
        *float_,
        *string,
        *path,
        *enum_,
        *enum_string
    );

    // Handle positional arguments.
    printf("positional arguments:");
    for (int i = 0; i < pos_args_len; i++) printf(" %s", pos_args[i]);
    printf("\n");

    // Frees all library memory, including options and positional arguments.
    free_args(&a);
    return EXIT_SUCCESS;
}
