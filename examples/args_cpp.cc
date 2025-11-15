#include "args.h"

static const char *VERSION = "v1.3.0";

static void print_help(ArgsCpp &a, const char *program_name) {
    // Print description and usage, then call `print_options`.
    printf("%s - Example of using 'args' library\n", program_name);
    printf("\n");
    printf("Usage:\n");
    printf("  %s [options]\n", program_name);
    // Shell completions can either be left hidden and generated on install,
    // or be exposed to user by printing it in usage and/or documentation.
    printf("  %s completion <bash|zsh|fish>\n", program_name);
    printf("\n");
    a.print_options();
}

int main(int argc, char **argv) {
    ArgsCpp args;

    // Help flag calls the provided callback (can be lambda) and exits.
    args.option_help(print_help);
    // Version flag prints the provided string and exits.
    args.option_version(VERSION);

    // Define options.
    // Option methods return a reference to option, which provides builder-style API
    // for additional configuration, such as short_name, default_value, required.
    // Options should be stored by reference as values will be set by `parse_args`.
    // Accessing values before will result in error.
    const auto &long_ = args.option_long("long", "A long option").short_name('l').default_value(5);
    const auto &float_ = args.option_float("float", "A float option").short_name('f').required();
    const auto &string = args.option_string("str", "A string option").short_name('s').default_value("string default");
    const auto &path = args.option_path("path", "A path option").short_name('p');
    const auto &hidden = args.option_flag("hidden", nullptr).hidden();

    const char *enum_values[] = {"first", "sed", "third", nullptr};
    const auto &e = args.option_enum("enum", "An enum option", enum_values).short_name('e').default_value(0);
    const auto &es = args.option_enum_string("enum-str", "A string enum option", enum_values).default_value("default");

    // Parse arguments. Sets option values and returns positional arguments.
    // Handles shell completion by printing to stdout and exiting.
    // Exits on error: invalid option value, missing required option, unknown option, etc.
    // Must be called before side effects or stdout output.
    char **pos_args;
    int pos_args_len = args.parse_args(argc, argv, pos_args);

    // Print help on subcommand.
    if (pos_args_len == 1 && strcmp(pos_args[0], "help") == 0) {
        // print_help(args, argv[0]);
        return EXIT_SUCCESS;
    }

    // Print version on subcommand.
    if (pos_args_len == 1 && strcmp(pos_args[0], "version") == 0) {
        printf("v1.3.0\n");
        return EXIT_SUCCESS;
    }

    // Values can be accessed using implicit conversion or `.value()`.
    printf(
        "options: l=%ld f=%f s=\"%s\" p=\"%s\" h=%s e=%ld E=\"%s\"\n",
        long_.value(),
        float_.value(),
        string.value(),
        path.value(),
        hidden ? "true" : "false",
        e.value(),
        es.value()
    );

    // Handle positional arguments.
    printf("Positional arguments:");
    for (int i = 0; i < pos_args_len; i++) printf(" %s", pos_args[i]);
    printf("\n");

    return EXIT_SUCCESS;
}
