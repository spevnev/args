#include "args.h"

int main(int argc, char **argv) {
    // Zero-initialize library state.
    Args a = {0};

    // Define options.
    bool *help = option_flag(&a, 'h', "help", "Show help");
    bool *version = option_flag(&a, 'v', "version", "Print version");
    long *l = option_long(&a, 'l', "long", "A long option", true, 0);
    float *f = option_float(&a, 'f', "float", "A float option", true, 0.0F);
    const char **s = option_str(&a, 's', "str", "A string option", true, NULL);
    const char **p = option_path(&a, 'p', "path", "A path option", true, NULL);

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
        printf("v1.1.0\n");
        free_args(&a);
        return EXIT_SUCCESS;
    }

    // Use option values.
    printf("options: l=%ld f=%f s=%s p=%s\n", *l, *f, *s, *p);

    // Handle positional arguments.
    printf("positional arguments:");
    for (int i = 0; i < pos_args_len; i++) printf(" %s", pos_args[i]);
    printf("\n");

    // Frees all library memory, including options and positional arguments.
    free_args(&a);
    return EXIT_SUCCESS;
}
