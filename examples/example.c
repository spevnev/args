#include "args.h"

int main(int argc, char **argv) {
    args a = {0};
    bool *help = option_flag(&a, 'h', "help", "Show help");
    bool *version = option_flag(&a, 'v', "version", "Print version");
    long *l = option_long(&a, 'l', "long", "A long option", true, 0);
    float *f = option_float(&a, 'f', "float", "A float option", true, 0.0F);
    const char **s = option_str(&a, 's', "str", "A string option", true, NULL);
    const char **p = option_path(&a, 'p', "path", "A path option", true, NULL);

    char **pos_args;
    int pos_args_len = parse_args(&a, argc, argv, &pos_args);

    if (*help || (pos_args_len == 1 && strcmp(pos_args[0], "help") == 0)) {
        printf("%s - Example of using 'args' library\n", argv[0]);
        printf("\n");
        printf("Usage:\n");
        printf("  %s [options]\n", argv[0]);
        printf("  %s completion <bash|zsh|fish>\n", argv[0]);
        printf("\n");
        print_options(&a, stdout);
        free_args(&a);
        return EXIT_SUCCESS;
    }

    if (*version || (pos_args_len == 1 && strcmp(pos_args[0], "version") == 0)) {
        printf("v1.1.0\n");
        free_args(&a);
        return EXIT_SUCCESS;
    }

    printf("options: l=%ld f=%f s=%s p=%s\n", *l, *f, *s, *p);

    printf("positional arguments:");
    for (int i = 0; i < pos_args_len; i++) printf(" %s", pos_args[i]);
    printf("\n");

    free_args(&a);
    return EXIT_SUCCESS;
}
