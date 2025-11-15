# args

*args* is a simple single-header library for parsing command-line arguments in C/C++.

## Example & Usage

<details>
    <summary>C</summary>

```c
// examples/args.c
#include "args.h"

static const char *VERSION = "v1.3.0";

static void print_help(Args *a, const char *program_name) {
    // Print description and usage, then call `print_options`.
    printf("%s - Example of using 'args' library\n", program_name);
    printf("\n");
    printf("Usage:\n");
    printf("  %s [options]\n", program_name);
    // Shell completions can either be left hidden and generated on install,
    // or be exposed to user by printing it in usage and/or documentation.
    printf("  %s completion <bash|zsh|fish>\n", program_name);
    printf("\n");
    print_options(a, stdout);
}

int main(int argc, char **argv) {
    // Zero-initialize library state.
    Args a = {0};

    // Help flag calls the provided callback and exits.
    option_help(&a, print_help);
    // Version flag prints the provided string and exits.
    option_version(&a, VERSION);

    // Define options.
    // All options take: args*, long name, description, and zero or more named arguments.
    // Named arguments are set through designated initializer: `.name1 = value1, .name2 = value2`,
    // with default value 0 / '\0' / false / NULL depending on the type.
    // Named arguments: short_name, required, hidden, default_value.
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
    const bool *hidden = option_flag(&a, "hidden", NULL, .hidden = true);

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
    // Exits on error: invalid option value, missing required option, unknown option, etc.
    // Must be called before side effects or stdout output.
    char **pos_args;
    int pos_args_len = parse_args(&a, argc, argv, &pos_args);

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
        "options: l=%ld f=%f s=\"%s\" p=\"%s\" h=%s e=%d E=\"%s\"\n",
        *long_,
        *float_,
        *string,
        *path,
        *hidden ? "true" : "false",
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
```
</details>

<details>
    <summary>C++</summary>

```cpp
// examples/args_cpp.cc
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
```
</details>

## Shell completion

Supported shells: bash, zsh, fish.

Completion options are generated *dynamically* by the executable which means:
1. Completion scripts do *not* need to be updated or reinstalled.
2. Executable *must* be in the PATH for completion to work.

Completions scripts should be installed through the build system, package manager, or explicitly by user. \
For system-wide installation:

```shell
PROGRAM_NAME completion bash > /usr/share/bash-completion/completions/PROGRAM_NAME
PROGRAM_NAME completion zsh > /usr/share/zsh/site-functions/_PROGRAM_NAME
PROGRAM_NAME completion fish > /usr/share/fish/vendor_completions.d/PROGRAM_NAME.fish
```

## Library options

| option                        | description                                                      | default     |
|-------------------------------|------------------------------------------------------------------|-------------|
| `ARGS_DISABLE_COMPLETION`     | Disable shell completion                                         | not defined |
| `ARGS_SKIP_UNKNOWN`           | Skip unknown options, otherwise it is an error                   | not defined |
| `ARGS_ALLOW_OVERWRITING`      | Allow overwriting option, otherwise it is an error               | not defined |
| `ARGS_PADDING`                | The number of spaces between the option name and its description | 2           |
| `ARGS_LINE_LENGTH`            | Line length                                                      | 80          |
| `ARGS_MIN_DESCRIPTION_LENGTH` | Minimum length of the description line, ignoring the line length | 30          |
| `ARGS_HIDE_DEFAULTS`          | Hides default values when printing options                       | not defined |

## Contributing

Configure git to use pre-commit hook to update README when examples change:

```console
$ git config --local core.hooksPath .githooks
```
