#include "args.h"

static const char *VERSION = "v1.3.0";

static void print_help(Args *a, const char *program_name) {
    // Print description and usage, then call `print_options`.
    printf("%s - Example of using 'args' library\n", program_name);
    printf("\n");
    printf("Usage:\n");
    printf("  %s [options]\n", program_name);
    // Document shell completion to allow users to install them manually.
    printf("  %s completion <bash|zsh|fish>\n", program_name);
    printf("\n");
    print_options(a, stdout);
}

int main(int argc, char **argv) {
    // Initialize library.
    Args a = {0};

    // Help flag calls the provided callback and exits.
    option_help(&a, print_help);
    // Version flag prints the provided string and exits.
    option_version(&a, VERSION);

    // Define options.
    const long *long_ = option_long(&a, "long", "A long option", .default_value = 5, .short_name = 'l');
    const float *float_ = option_float(&a, "float", "A float option", .short_name = 'f', .required = true);
    const char **string = option_string(
        &a,
        "str",
        "A string option",
        .short_name = 's',
        .default_value = "string default"
    );
    const char **path = option_path(&a, "path", "A path option");

    // Declare a hidden command-like option, e.g. dump system info for bug report.
    // Since handling of this flag (see below) ignores values of other options,
    // `early_exit` can be set to skip validation of other options when present.
    // This way, flag works without required options or with invalid options/values.
    const bool *dump = option_flag(&a, "dump", NULL, .hidden = true, .early_exit = true);

    // If enum is continuous and array matches it, result of `option_enum` can be casted directly.
    typedef enum { FIRST, SECOND, THIRD } Enum;
    const char *enum_values[] = {"first", "second", "third", NULL};
    const Enum *enum_ = (const Enum *)
        option_enum(&a, "enum", "An enum option", enum_values, .short_name = 'e', .default_value = FIRST);
    // If values don't match, or the enum isn't continuous, it may be desirable to get a string instead.
    // Values are defined in the arguments, they must be wrapped in parenthesis.
    const char **enum_string = option_enum_string(
        &a,
        "enum-str",
        "A string enum option",
        ((const char *[]) {"other", "enum", "values", NULL}),
        .default_value = "default"
    );

    // Parse arguments. Sets option values and returns positional arguments.
    char **pos_args;
    int pos_args_len = parse_args(&a, argc, argv, &pos_args);

    // Immediately handle flag with `early_exit`.
    if (*dump) {
        printf("--dump is set!\n");
        free_args(&a);
        return EXIT_SUCCESS;
    }

    // Print help on subcommand.
    if (pos_args_len == 1 && strcmp(pos_args[0], "help") == 0) {
        print_help(&a, argv[0]);
        free_args(&a);
        return EXIT_SUCCESS;
    }

    // Print version on subcommand.
    if (pos_args_len == 1 && strcmp(pos_args[0], "version") == 0) {
        printf("%s\n", VERSION);
        free_args(&a);
        return EXIT_SUCCESS;
    }

    // Use option values.
    printf(
        "options: l=%ld f=%f s=\"%s\" p=\"%s\" e=%d E=\"%s\"\n",
        *long_,
        *float_,
        *string,
        *path,
        *enum_,
        *enum_string
    );

    // Handle positional arguments.
    printf("Positional arguments:");
    for (int i = 0; i < pos_args_len; i++) printf(" %s", pos_args[i]);
    printf("\n");

    // Frees all library memory, including options and positional arguments.
    free_args(&a);
    return EXIT_SUCCESS;
}
