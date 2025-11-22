#include "args.h"

static const char *VERSION = "v2.0.0";

static void print_help(ArgsCpp &args, const char *program_name) {
    // Print description and usage, then call `print_options`.
    printf("%s - Example of using 'args' library in C++\n", program_name);
    printf("\n");
    printf("Usage:\n");
    printf("  %s [options]\n", program_name);
    // Document shell completion to allow users to install them manually.
    printf("  %s completion <bash|zsh|fish>\n", program_name);
    printf("\n");
    args.print_options();
}

int main(int argc, char **argv) {
    // Initialize library.
    ArgsCpp args;

    // Help flag calls the provided callback and exits.
    args.option_help(print_help);
    // Version flag prints the provided string and exits.
    args.option_version(VERSION);

    // Define options.
    const auto &long_ = args.option_long("long", "A long option").default_value(5).short_name('l');
    const auto &float_ = args.option_float("float", "A float option").short_name('f').required();
    const auto &string = args.option_string("str", "A string option").short_name('s').default_value("string default");
    const auto &path = args.option_path("path", "A path option");

    // Declare a hidden command-like option, e.g. dump system info for bug report.
    // Since handling of this flag (see below) ignores values of other options,
    // `early_exit` can be set to skip validation of other options when present.
    // This way, flag works without required options or with invalid options/values.
    const auto &dump = args.option_flag("dump", nullptr).hidden().early_exit();

    // If enum is continuous and array matches it, result of `option_enum` can be casted directly.
    typedef enum { FIRST, SECOND, THIRD } Enum;
    const char *enum_one[] = {"first", "second", "third", NULL};
    const auto &enum_ = args.option_enum("enum", "An enum option", enum_one).short_name('e').default_value(FIRST);
    // If values don't match, or the enum isn't continuous, it may be desirable to get a string instead.
    // Values are defined in the arguments, they must be wrapped in parenthesis.
    const char *enum_two[] = {"other", "enum", "values", NULL};
    const auto &enum_string = args.option_enum_string("enum-str", "A string enum option", enum_two)
                                  .default_value("default");

    // Parse arguments. Sets option values and returns positional arguments.
    char **pos_args;
    int pos_args_len = args.parse_args(argc, argv, pos_args);

    // Immediately handle flag with `early_exit`.
    if (dump) {
        printf("--dump is set!\n");
        return EXIT_SUCCESS;
    }

    // Print help on subcommand.
    if (pos_args_len == 1 && strcmp(pos_args[0], "help") == 0) {
        print_help(args, argv[0]);
        return EXIT_SUCCESS;
    }

    // Print version on subcommand.
    if (pos_args_len == 1 && strcmp(pos_args[0], "version") == 0) {
        printf("%s\n", VERSION);
        return EXIT_SUCCESS;
    }

    // Use option values.
    printf(
        "options: l=%ld f=%f s=\"%s\" p=\"%s\" e=%lu E=\"%s\"\n",
        long_.value(),
        float_.value(),
        string.value(),
        path.value(),
        enum_.value(),
        enum_string.value()
    );

    // Handle positional arguments.
    printf("Positional arguments:");
    for (int i = 0; i < pos_args_len; i++) printf(" %s", pos_args[i]);
    printf("\n");

    return EXIT_SUCCESS;
}
