# args
*args* is a simple single-header library for parsing command-line arguments in C.

## Example
```c
// examples/example.c
#include "args.h"

int main(int argc, char **argv) {
    args a = {0};
    bool *help = option_flag(&a, 'h', "help", "Show help");
    long *l = option_long(&a, 'l', "long", "A long option", true, 0);
    float *f = option_float(&a, 'f', "float", "A float option", true, 0.0F);
    const char **s = option_str(&a, 's', "str", "A string option", true, NULL);

    char **pos_args;
    int pos_args_len = parse_args(&a, argc, argv, &pos_args);

    if (*help) {
        printf("%s - Example of using args\n", argv[0]);
        printf("Usage: %s [options]\n", argv[0]);
        printf("\n");
        print_options(&a, stdout);
        free_args(&a);
        return EXIT_SUCCESS;
    }

    printf("options: l=%ld f=%f s=%s\n", *l, *f, *s);

    printf("positional arguments:");
    for (int i = 0; i < pos_args_len; i++) printf(" %s", pos_args[i]);
    printf("\n");

    free_args(&a);
    return EXIT_SUCCESS;
}
```

## Library options

option | description | default
-|-|-
`ARGS_SKIP_UNKNOWN` | Skip unknown options, otherwise it is an error | not defined
`ARGS_ALLOW_OVERWRITING` | Allow overwriting option, otherwise it is an error | not defined
`ARGS_PADDING` | The number of spaces between the option name and its description | 2
`ARGS_LINE_LENGTH` | Line length | 80
`ARGS_MIN_DESC_LENGTH` | Minimum length of the description line, ignoring the line length | 30
