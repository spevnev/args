#include "args.h"

int main(int argc, char **argv) {
    ArgsCpp args;

    // Define options.
    const auto &help = args.option_flag('h', "help", "Show help", true);
    const auto &version = args.option_flag('v', "version", "Print version", true);
    const auto &l = args.option_long('l', "long", "A long option");
    const auto &f = args.option_float('f', "float", "A float option");
    const auto &s = args.option_string('s', "str", "A string option");
    const auto &p = args.option_path('p', "path", "A path option");

    const char *enum_values[] = {"first", "sed", "third", nullptr};
    const auto &e = args.option_enum('e', "enum", "An enum option", enum_values);
    const auto &es = args.option_enum_string('\0', "enum-str", "A string enum option", enum_values, false, "default");

    // Parse arguments. Sets option values and returns positional arguments.
    // Handles shell completion by printing to stdout and exiting.
    // Exits on errors: wrong option value, missing required option, unknown option, etc.
    // Must be called before side effects and printing to stdout.
    char **pos_args;
    int pos_args_len = args.parse_args(argc, argv, pos_args);

    // Print help on flag or subcommand.
    if (help || (pos_args_len == 1 && strcmp(pos_args[0], "help") == 0)) {
        // Print description and usage, then call `print_options`.
        printf("%s - Example of using 'args' library\n", argv[0]);
        printf("\n");
        printf("Usage:\n");
        printf("  %s [options]\n", argv[0]);
        // Shell completions can either be left hidden and generated on install,
        // or be exposed to user by printing it in usage and/or documentation.
        printf("  %s completion <bash|zsh|fish>\n", argv[0]);
        printf("\n");
        args.print_options();
        return EXIT_SUCCESS;
    }

    // Similarly, print version on flag or subcommand.
    if (version || (pos_args_len == 1 && strcmp(pos_args[0], "version") == 0)) {
        printf("v1.3.0\n");
        return EXIT_SUCCESS;
    }

    // Use option values.
    printf("options: l=%ld f=%f s=\"%s\" p=\"%s\" e=%ld E=\"%s\"\n", l, f, s, p, e, es);

    // Handle positional arguments.
    printf("positional arguments:");
    for (int i = 0; i < pos_args_len; i++) printf(" %s", pos_args[i]);
    printf("\n");

    return EXIT_SUCCESS;
}
