// args v1.1.0
// Documentation, examples, and issues: https://github.com/spevnev/args

// MIT License
//
// Copyright (c) 2025 Serhii Pievniev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifdef ARGS_H_
#error args must be included only once
#else
#define ARGS_H_
#endif

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef ARGS_PADDING
#define ARGS_PADDING 2
#endif

#ifndef ARGS_LINE_LENGTH
#define ARGS_LINE_LENGTH 80
#endif

#ifndef ARGS_MIN_DESC_LENGTH
#define ARGS_MIN_DESC_LENGTH 30
#endif

typedef struct args_option {
    struct args_option *next;
    char short_name;
    char *long_name;
    char *description;
    bool is_optional;
    bool is_set;
    enum {
        ARGS_TYPE_LONG,
        ARGS_TYPE_FLOAT,
        ARGS_TYPE_STR,
        ARGS_TYPE_BOOL,
    } type;
    struct {
        long long_;
        float float_;
        char *str;
        bool bool_;
    } value;
} args_option;

typedef struct {
    args_option *head;
    args_option *tail;
    char **pos_args;
    size_t max_descr_len;
} args;

#if defined(__has_attribute) && __has_attribute(unused)
#define ARGS_MAYBE_UNUSED __attribute__((unused))
#else
#define ARGS_MAYBE_UNUSED
#endif

#if defined(__has_attribute) && __has_attribute(warn_unused_result)
#define ARGS_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
#define ARGS_WARN_UNUSED_RESULT
#endif

#define ARGS_FATAL(...)               \
    do {                              \
        fprintf(stderr, "ERROR: ");   \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, ".\n");       \
        exit(EXIT_FAILURE);           \
    } while (0)

#define ARGS_OUT_OF_MEMORY() ARGS_FATAL("Process ran out of memory")
#define ARGS_UNREACHABLE() ARGS_FATAL("Unreachable")

#define ARGS_ASSERT(condition)                                                         \
    do {                                                                               \
        if (!(condition)) {                                                            \
            ARGS_FATAL("Assert (%s) failed at %s:%d", #condition, __FILE__, __LINE__); \
        }                                                                              \
    } while (0)

static char *args_strdup(const char *str) {
    ARGS_ASSERT(str != NULL);
    size_t len = strlen(str);
    char *dup = malloc(len + 1);
    if (dup == NULL) ARGS_OUT_OF_MEMORY();
    memcpy(dup, str, len);
    dup[len] = '\0';
    return dup;
}

static args_option *args_new_option(args *a, char short_name, const char *long_name, const char *description,
                                    bool is_optional) {
    ARGS_ASSERT(a != NULL);

    if (short_name != '\0' && !isalnum(short_name)) {
        ARGS_FATAL("Invalid short name '%c'. It must be alphanumeric", short_name);
    }

    if (long_name == NULL) ARGS_FATAL("Option must have a long name");
    for (const char *c = long_name; *c != '\0'; c++) {
        if (isalnum(*c) || *c == '_' || *c == '+') continue;
        if (*c == '-' && c != long_name) continue;
        ARGS_FATAL("Invalid long name \"%s\". It must consist of alphanumerics and \"-_+\", cannot start with '-'",
                   long_name);
    }

    if (description != NULL) {
        for (const char *c = description; *c != '\0'; c++) {
            if (*c == '\n') ARGS_FATAL("Description must not contain newlines. It will be split automatically");
            if (*c == '\t') ARGS_FATAL("Description must not contain tabs to maintain proper length");
            if (!isprint(*c)) ARGS_FATAL("Description of \"%s\" contains an unprintable character '%c'", long_name, *c);
        }

        size_t length = strlen(description);
        if (length > a->max_descr_len) a->max_descr_len = length;
    }

    args_option *option = (args_option *) malloc(sizeof(*option));
    if (option == NULL) ARGS_OUT_OF_MEMORY();
    option->next = NULL;
    option->short_name = short_name;
    option->long_name = args_strdup(long_name);
    option->description = description == NULL ? NULL : args_strdup(description);
    option->is_optional = is_optional;
    option->is_set = false;

    if (a->head == NULL) {
        a->head = option;
    } else {
        a->tail->next = option;
    }
    a->tail = option;
    return option;
}

static void args_parse_value(args_option *option, const char *value) {
    ARGS_ASSERT(option != NULL && value != NULL);
    switch (option->type) {
        case ARGS_TYPE_LONG: {
            char *end = NULL;
            option->value.long_ = strtol(value, &end, 0);
            if (end == NULL || *end != '\0') ARGS_FATAL("Invalid integer \"%s\"", value);
        } break;
        case ARGS_TYPE_FLOAT: {
            char *end = NULL;
            option->value.float_ = strtof(value, &end);
            if (end == NULL || *end != '\0') ARGS_FATAL("Invalid float \"%s\"", value);
        } break;
        case ARGS_TYPE_STR:  option->value.str = args_strdup(value); break;
        case ARGS_TYPE_BOOL: ARGS_UNREACHABLE(); break;
    }
}

#ifndef ARGS_DISABLE_COMPLETION
static void args_completion_bash_print(const char *program_name) {
    ARGS_ASSERT(program_name != NULL);
    printf(
        "_%s() {\n"
        "    local cur prev words cword\n"
        "    _init_completion || return\n"
        "\n"
        "    if [[ $cur == -* ]]; then\n"
        "        COMPREPLY=($(compgen -W \"$(%s __complete bash 2>/dev/null)\" -- \"$cur\"))\n"
        "    fi\n"
        "}\n"
        "\n"
        "complete -o default -F _%s %s\n",
        program_name, program_name, program_name, program_name);
}

static void args_completion_zsh_print(const char *program_name) {
    ARGS_ASSERT(program_name != NULL);
    printf(
        "#compdef %s\n"
        "\n"
        "_%s() {\n"
        "    local IFS=$'\\n'\n"
        "    _arguments $(%s __complete zsh 2>/dev/null)\n"
        "}\n"
        "\n"
        "_%s\n",
        program_name, program_name, program_name, program_name);
}

static void args_completion_fish_print(const char *program_name) {
    ARGS_ASSERT(program_name != NULL);
    printf(
        "complete -c %s -e\n"
        "\n"
        "for args in (%s __complete fish 2>/dev/null)\n"
        "    eval \"complete -c %s $args\"\n"
        "end\n",
        program_name, program_name, program_name);
}

static void args_completion_bash_complete(args *a) {
    ARGS_ASSERT(a != NULL);
    for (args_option *i = a->head; i != NULL; i = i->next) {
        if (i->short_name != '\0') printf("-%c ", i->short_name);
        printf("--%s ", i->long_name);
    }
    printf("\n");
}

static void args_completion_zsh_complete(args *a) {
    ARGS_ASSERT(a != NULL);

    // Double the size in the extreme case that description consists entirely of escaped characters.
    char *buffer = malloc(a->max_descr_len * 2 + 1);
    if (buffer == NULL) ARGS_OUT_OF_MEMORY();
    for (args_option *i = a->head; i != NULL; i = i->next) {
        const char *description = "";
        if (i->description != NULL) {
            // Escape ']' and '\' in description, and wrap in square brackets as format requires.
            char *c = buffer;
            *c++ = '[';
            for (char *j = i->description; *j != '\0'; j++) {
                if (*j == ']' || *j == '\\') *c++ = '\\';
                *c++ = *j;
            }
            *c++ = ']';
            *c++ = '\0';
            description = buffer;
        }

        if (i->short_name == '\0') {
            printf("--%s=%s\n", i->long_name, description);
        } else {
            printf("(-%c --%s)-%c%s\n", i->short_name, i->long_name, i->short_name, description);
            printf("(-%c --%s)--%s=%s\n", i->short_name, i->long_name, i->long_name, description);
        }
    }
    printf("*:file:_files\n");
    free(buffer);
}

static void args_completion_fish_complete(args *a) {
    ARGS_ASSERT(a != NULL);

    // Double the size in the extreme case that description consists entirely of escaped characters.
    char *buffer = malloc(a->max_descr_len * 2 + 1);
    if (buffer == NULL) ARGS_OUT_OF_MEMORY();
    for (args_option *i = a->head; i != NULL; i = i->next) {
        printf("-l %s", i->long_name);
        if (i->short_name != '\0') printf(" -s %c", i->short_name);
        if (i->description != NULL) {
            // Escape '$', '"' and '\'.
            char *c = buffer;
            for (char *j = i->description; *j != '\0'; j++) {
                if (*j == '$' || *j == '"' || *j == '\\') *c++ = '\\';
                *c++ = *j;
            }
            *c++ = '\0';

            printf(" -d \"%s\"", buffer);
        }
        printf("\n");
    }
    free(buffer);
}
#endif

// Frees all the memory, including option values and array of positional arguments.
static void free_args(args *a) {
    if (a == NULL) return;

    args_option *current = a->head;
    while (current != NULL) {
        args_option *next = current->next;
        free(current->long_name);
        free(current->description);
        if (current->type == ARGS_TYPE_STR) free(current->value.str);
        free(current);
        current = next;
    }
    a->head = NULL;
    a->tail = NULL;

    free(a->pos_args);
    a->pos_args = NULL;
}

// Defines a long option, returns a pointer set by `parse_args`.
// Use '\0' for no short name.
// Exits if `a` or `long_name` is NULL, or out of memory.
ARGS_MAYBE_UNUSED ARGS_WARN_UNUSED_RESULT static long *option_long(args *a, char short_name, const char *long_name,
                                                                   const char *description, bool is_optional,
                                                                   long default_value) {
    ARGS_ASSERT(a != NULL);
    args_option *option = args_new_option(a, short_name, long_name, description, is_optional);
    option->type = ARGS_TYPE_LONG;
    option->value.long_ = default_value;
    return &option->value.long_;
}

// Defines a float option, returns a pointer set by `parse_args`.
// Use '\0' for no short name.
// Exits if `a` or `long_name` is NULL, or out of memory.
ARGS_MAYBE_UNUSED ARGS_WARN_UNUSED_RESULT static float *option_float(args *a, char short_name, const char *long_name,
                                                                     const char *description, bool is_optional,
                                                                     float default_value) {
    ARGS_ASSERT(a != NULL);
    args_option *option = args_new_option(a, short_name, long_name, description, is_optional);
    option->type = ARGS_TYPE_FLOAT;
    option->value.float_ = default_value;
    return &option->value.float_;
}

// Defines a string option, returns a pointer set by `parse_args`.
// String memory is owned by library, freed by `free_args`.
// Use '\0' for no short name.
// Exits if `a` or `long_name` is NULL, or out of memory.
ARGS_MAYBE_UNUSED ARGS_WARN_UNUSED_RESULT static const char **option_str(args *a, char short_name,
                                                                         const char *long_name, const char *description,
                                                                         bool is_optional, const char *default_value) {
    ARGS_ASSERT(a != NULL);
    args_option *option = args_new_option(a, short_name, long_name, description, is_optional);
    option->type = ARGS_TYPE_STR;
    option->value.str = default_value == NULL ? NULL : args_strdup(default_value);
    return (const char **) &option->value.str;
}

// Defines a boolean flag, returns a pointer set by `parse_args`.
// Use '\0' for no short name.
// Exits if `a` or `long_name` is NULL, or out of memory.
ARGS_MAYBE_UNUSED ARGS_WARN_UNUSED_RESULT static bool *option_flag(args *a, char short_name, const char *long_name,
                                                                   const char *description) {
    ARGS_ASSERT(a != NULL);
    args_option *option = args_new_option(a, short_name, long_name, description, true);
    option->type = ARGS_TYPE_BOOL;
    option->value.bool_ = false;
    return &option->value.bool_;
}

// Parses arguments, sets option-returned values.
// Unless disabled, handles shell completion by writing to stdout and exiting.
// Must be called before side effects or stdout output.
// Returns positional arguments via `pos_args`, and their count as return value.
// Elements are from `argv`, while the array memory is managed by library.
// On error, prints to stderr and exits.
static int parse_args(args *a, int argc, char **argv, char ***pos_args) {
    ARGS_ASSERT(a != NULL && argv != NULL && pos_args != NULL);

    ARGS_ASSERT(argc >= 0);
    if (argc == 0) ARGS_FATAL("Expected the first argument to be a program name");
    const char *program_name = argv[0];
    argc--;
    argv++;

    for (args_option *i = a->head; i != NULL; i = i->next) {
        for (args_option *j = i->next; j != NULL; j = j->next) {
            if (i->short_name == j->short_name && i->short_name != '\0') {
                ARGS_FATAL("Duplicate short name '%c' in options \"%s\" and \"%s\"",  //
                           i->short_name, i->long_name, j->long_name);
            }

            if (strcmp(i->long_name, j->long_name) == 0) ARGS_FATAL("Duplicate option \"%s\"", i->long_name);
        }
    }

#ifdef ARGS_DISABLE_COMPLETION
    (void) program_name;
#else
    if (argc >= 1 && strcmp(argv[0], "completion") == 0) {
        if (argc == 1) ARGS_FATAL("Command 'completion' requires an argument: bash, zsh, fish");

        for (const char *c = program_name; *c != '\0'; c++) {
            if (isalnum(*c) || *c == '_' || *c == '.' || *c == '+' || *c == ':') continue;
            if (*c == '-' && c != program_name) continue;
            ARGS_FATAL("Invalid program name \"%s\"", program_name);
        }

        if (strcmp(argv[1], "bash") == 0) {
            args_completion_bash_print(program_name);
        } else if (strcmp(argv[1], "zsh") == 0) {
            args_completion_zsh_print(program_name);
        } else if (strcmp(argv[1], "fish") == 0) {
            args_completion_fish_print(program_name);
        } else {
            ARGS_FATAL("Failed to generate completion script: unknown shell \"%s\"", argv[1]);
        }
        free_args(a);
        exit(EXIT_SUCCESS);
    }

    if (argc >= 1 && strcmp(argv[0], "__complete") == 0) {
        if (argc == 1) ARGS_FATAL("Command '__complete' requires an argument: bash, zsh, fish");

        if (strcmp(argv[1], "bash") == 0) {
            args_completion_bash_complete(a);
        } else if (strcmp(argv[1], "zsh") == 0) {
            args_completion_zsh_complete(a);
        } else if (strcmp(argv[1], "fish") == 0) {
            args_completion_fish_complete(a);
        } else {
            ARGS_FATAL("Failed to generate completions: unknown shell \"%s\"", argv[1]);
        }
        free_args(a);
        exit(EXIT_SUCCESS);
    }
#endif

    int pos_args_idx = 0;
    a->pos_args = malloc(sizeof(*argv) * argc);
    if (a->pos_args == NULL) ARGS_OUT_OF_MEMORY();
    *pos_args = a->pos_args;

    while (argc > 0) {
        char *arg = *argv;
        size_t arg_len = strlen(arg);
        argc--;
        argv++;

        if (arg_len < 2 || arg[0] != '-') {
            (*pos_args)[pos_args_idx++] = arg;
            continue;
        }

        if (arg[1] == '-') {
            arg += 2;
            arg_len -= 2;

            args_option *option = a->head;
            size_t option_len;
            while (option != NULL) {
                option_len = strlen(option->long_name);
                if (strncmp(arg, option->long_name, option_len) == 0
                    && (arg[option_len] == '\0' || arg[option_len] == '=')) {
                    break;
                }
                option = option->next;
            }
#ifdef ARGS_SKIP_UNKNOWN
            if (option == NULL) continue;
#else
            if (option == NULL) ARGS_FATAL("Unknown or invalid option \"%s\"", arg);
#endif

#ifndef ARGS_ALLOW_OVERWRITING
            if (option->is_set) ARGS_FATAL("Option \"%s\" is set more than once", option->long_name);
#endif
            option->is_set = true;

            if (option->type == ARGS_TYPE_BOOL) {
                if (arg[option_len] == '=') ARGS_FATAL("Flags cannot have a value: \"%s\"", arg);
                option->value.bool_ = true;
                continue;
            }

            const char *value;
            if (arg[option_len] == '=') {
                value = arg + option_len + 1;
            } else {
                if (argc == 0) ARGS_FATAL("Option \"%s\" is missing a value", option->long_name);
                value = *argv;
                argc--;
                argv++;
            }

            args_parse_value(option, value);
        } else {
            arg += 1;
            arg_len -= 1;

            if (arg_len != 1) ARGS_FATAL("Short option must be separate");
            char ch = *arg;

            args_option *option = a->head;
            while (option != NULL && option->short_name != ch) option = option->next;

#ifdef ARGS_SKIP_UNKNOWN
            if (option == NULL) continue;
#else
            if (option == NULL) ARGS_FATAL("Unknown or invalid option \"%s\"", arg);
#endif

#ifndef ARGS_ALLOW_OVERWRITING
            if (option->is_set) ARGS_FATAL("Option '%c' is set more than once", option->short_name);
#endif
            option->is_set = true;

            if (option->type == ARGS_TYPE_BOOL) {
                option->value.bool_ = true;
                continue;
            }

            if (argc == 0) ARGS_FATAL("Option '%c' is missing a value", option->short_name);
            const char *value = *argv;
            argc--;
            argv++;

            args_parse_value(option, value);
        }
    }

    for (args_option *option = a->head; option != NULL; option = option->next) {
        if (!option->is_optional && !option->is_set) ARGS_FATAL("Missing a required option \"%s\"", option->long_name);
    }

    return pos_args_idx;
}

// Prints all options to `fp`.
// Caller is responsible for printing usage, as well as `completion` command.
ARGS_MAYBE_UNUSED static void print_options(args *a, FILE *fp) {
    ARGS_ASSERT(a != NULL && fp != NULL);

    size_t longest_option = 0;
    for (args_option *option = a->head; option != NULL; option = option->next) {
        size_t length = strlen(option->long_name);
        if (length > longest_option) longest_option = length;
    }

    fprintf(fp, "Options:\n");
    for (args_option *option = a->head; option != NULL; option = option->next) {
        fprintf(fp, "  ");

        if (option->short_name != '\0') {
            fprintf(fp, "-%c, ", option->short_name);
        } else {
            fprintf(fp, "    ");
        }
        fprintf(fp, "--%s", option->long_name);

        int length_diff = longest_option - strlen(option->long_name);
        fprintf(fp, "%*c", length_diff + ARGS_PADDING, ' ');

        // Print description and break into multiple lines if needed.
        if (option->description != NULL) {
            int offset = 8 + longest_option + ARGS_PADDING;
            int line_length = ARGS_LINE_LENGTH - offset;
            if (line_length < ARGS_MIN_DESC_LENGTH) line_length = ARGS_MIN_DESC_LENGTH;
            int length = strlen(option->description);
            bool is_multiline = length > line_length;
            char *cur = option->description;
            while (length > line_length) {
                // Find the closest space to break the line.
                int chunk_length = line_length;
                while (chunk_length > 0 && cur[chunk_length] != ' ') chunk_length--;
                // If failed to find a space or it is too early, break mid-word.
                if (chunk_length < line_length / 2) chunk_length = line_length;

                fprintf(fp, "%.*s\n%*c", chunk_length, cur, offset, ' ');

                // Advance by one more to skip the space.
                cur += chunk_length + 1;
                length -= chunk_length + 1;
            }
            fprintf(fp, "%s", cur);

            if (is_multiline) {
                // Print description on the new line to avoid breaking it too.
                fprintf(fp, "\n%*c", offset, ' ');
            } else {
                fprintf(fp, " ");
            }
        }

        if (option->is_optional) {
            fprintf(fp, "(default: ");
            switch (option->type) {
                case ARGS_TYPE_LONG:  fprintf(fp, "%ld", option->value.long_); break;
                case ARGS_TYPE_FLOAT: fprintf(fp, "%.3f", option->value.float_); break;
                case ARGS_TYPE_STR:
                    if (option->value.str == NULL) {
                        fprintf(fp, "none");
                    } else {
                        fprintf(fp, "%s", option->value.str);
                    }
                    break;
                case ARGS_TYPE_BOOL: fprintf(fp, "%s", option->value.bool_ ? "true" : "false"); break;
            }
            fprintf(fp, ")");
        }
        fprintf(fp, "\n");
    }
}

#undef ARGS_MAYBE_UNUSED
#undef ARGS_WARN_UNUSED_RESULT
#undef ARGS_FATAL
#undef ARGS_OUT_OF_MEMORY
#undef ARGS_UNREACHABLE
#undef ARGS_ASSERT
