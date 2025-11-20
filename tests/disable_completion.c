#define ARGS_DISABLE_COMPLETION
#include <assert.h>
#include <unistd.h>
#include "args.h"

static void failure(void) { _exit(EXIT_FAILURE); }

//< completion bash
int main(int argc, char **argv) {
    atexit(failure);

    Args a = {0};

    char **pos_args;
    int pos_args_len = parse_args(&a, argc, argv, &pos_args);

    assert(pos_args_len == 2);
    assert(strcmp(pos_args[0], "completion") == 0);
    assert(strcmp(pos_args[1], "bash") == 0);

    _exit(EXIT_SUCCESS);
}
