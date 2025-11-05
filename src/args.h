// args v1.2.0
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

// ====================================================================================================================
//                                                  IMPLEMENTATION
// ====================================================================================================================

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
#include <strings.h>

#ifndef ARGS_PADDING
#define ARGS_PADDING 2
#endif

#ifndef ARGS_LINE_LENGTH
#define ARGS_LINE_LENGTH 80
#endif

#ifndef ARGS_MIN_DESC_LENGTH
#define ARGS_MIN_DESC_LENGTH 30
#endif

typedef enum {
    ARGS__TYPE_LONG,
    ARGS__TYPE_FLOAT,
    ARGS__TYPE_STR,
    ARGS__TYPE_PATH,
    ARGS__TYPE_BOOL,
    ARGS__TYPE_ENUM_IDX,
    ARGS__TYPE_ENUM_STR,
} Args__Type;

typedef struct Args__Option {
    struct Args__Option *next;
    char short_name;
    char *long_name;
    char *description;
    bool is_optional;
    bool is_set;
    Args__Type type;
    union {
        long long_;
        float float_;
        char *str;
        char *enum_;
    } default_value;
    union {
        long long_;
        float float_;
        char *str;
        bool bool_;
        struct {
            char **values;
            unsigned int length;
            union {
                size_t index;
                const char *value;
            } as;
        } enum_;
    } value;
} Args__Option;

typedef struct {
    Args__Option *head;
    Args__Option *tail;
    char **pos_args;
    size_t max_descr_len;
} Args;

#ifdef __has_attribute
#define ARGS__HAS_ATTRIBUTE(attribute) __has_attribute(attribute)
#else
#define ARGS__HAS_ATTRIBUTE(attribute) 0
#endif

#if ARGS__HAS_ATTRIBUTE(unused)
#define ARGS__MAYBE_UNUSED __attribute__((unused))
#else
#define ARGS__MAYBE_UNUSED
#endif

#if ARGS__HAS_ATTRIBUTE(warn_unused_result)
#define ARGS__WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
#define ARGS__WARN_UNUSED_RESULT
#endif

#define ARGS__FATAL(...)              \
    do {                              \
        fprintf(stderr, "ERROR: ");   \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, ".\n");       \
        exit(EXIT_FAILURE);           \
    } while (0)

#define ARGS__OUT_OF_MEMORY() ARGS__FATAL("Process ran out of memory")
#define ARGS__UNREACHABLE() ARGS__FATAL("Unreachable")

#define ARGS__ASSERT(condition)                                                         \
    do {                                                                                \
        if (!(condition)) {                                                             \
            ARGS__FATAL("Assert (%s) failed at %s:%d", #condition, __FILE__, __LINE__); \
        }                                                                               \
    } while (0)

static char *args__strdup(const char *str) {
    ARGS__ASSERT(str != NULL);
    size_t len = strlen(str);
    char *dup = (char *) malloc(len + 1);
    if (dup == NULL) ARGS__OUT_OF_MEMORY();
    memcpy(dup, str, len);
    dup[len] = '\0';
    return dup;
}

static const char *args__basename(const char *path) {
    ARGS__ASSERT(path != NULL);

    const char *filename = strrchr(path, '/');
#ifdef _WIN32
    const char *win_filename = strrchr(path, '\\');
    // Windows can use either slash, take the last one.
    if (filename == NULL || (win_filename != NULL && win_filename > filename)) filename = win_filename;
#endif
    return filename == NULL ? path : filename + 1;
}

static Args__Option *args__new_option(
    Args *a,
    char short_name,
    const char *long_name,
    const char *description,
    bool is_optional,
    Args__Type type
) {
    ARGS__ASSERT(a != NULL);

    if (short_name != '\0' && !isalnum(short_name)) {
        ARGS__FATAL("Invalid short name '%c'. It must be alphanumeric", short_name);
    }

    if (long_name == NULL) ARGS__FATAL("Option must have a long name");
    for (const char *c = long_name; *c != '\0'; c++) {
        if (isalnum(*c) || *c == '_' || *c == '+') continue;
        if (*c == '-' && c != long_name) continue;
        ARGS__FATAL(
            "Invalid long name \"%s\". It must consist of alphanumerics and \"-_+\", and cannot start with '-'",
            long_name
        );
    }

    if (description != NULL) {
        for (const char *c = description; *c != '\0'; c++) {
            if (*c == '\n') continue;
            if (*c == '\t') ARGS__FATAL("Description must not contain tabs to maintain proper length");
            if (!isprint(*c)) ARGS__FATAL("Description of \"%s\" contains an invalid character 0x%x", long_name, *c);
        }

        size_t length = strlen(description);
        if (length > a->max_descr_len) a->max_descr_len = length;
    }

    Args__Option *option = (Args__Option *) malloc(sizeof(*option));
    if (option == NULL) ARGS__OUT_OF_MEMORY();
    option->next = NULL;
    option->short_name = short_name;
    option->long_name = args__strdup(long_name);
    option->description = description == NULL ? NULL : args__strdup(description);
    option->is_optional = is_optional;
    option->is_set = false;
    option->type = type;

    if (a->head == NULL) {
        a->head = option;
    } else {
        a->tail->next = option;
    }
    a->tail = option;
    return option;
}

static void args__set_enum_values(Args__Option *option, const char **values) {
    ARGS__ASSERT(option != NULL && values != NULL);

    size_t length = 0;
    while (values[length] != NULL) length++;
    option->value.enum_.length = length;

    char **dup = (char **) malloc(sizeof(*dup) * length);
    if (dup == NULL) ARGS__OUT_OF_MEMORY();
    option->value.enum_.values = dup;

    for (size_t i = 0; i < length; i++) {
        for (const char *c = values[i]; *c != '\0'; c++) {
            if (!isprint(*c)) {
                ARGS__FATAL(
                    "Enum value of \"%s\" contains an invalid character 0x%x. "
                    "Maybe enum array is missing a NULL-terminator",
                    option->long_name, *c
                );
            }
        }
        dup[i] = args__strdup(values[i]);
    }
}

static void args__parse_value(Args__Option *option, const char *value) {
    ARGS__ASSERT(option != NULL && value != NULL);
    switch (option->type) {
        case ARGS__TYPE_LONG: {
            char *end = NULL;
            option->value.long_ = strtol(value, &end, 0);
            if (end == NULL || *end != '\0') ARGS__FATAL("Invalid integer \"%s\"", value);
        } break;
        case ARGS__TYPE_FLOAT: {
            char *end = NULL;
            option->value.float_ = strtof(value, &end);
            if (end == NULL || *end != '\0') ARGS__FATAL("Invalid float \"%s\"", value);
        } break;
        case ARGS__TYPE_STR:
        case ARGS__TYPE_PATH:
            free(option->value.str);
            option->value.str = args__strdup(value);
            break;
        case ARGS__TYPE_BOOL: ARGS__UNREACHABLE(); break;
        case ARGS__TYPE_ENUM_IDX:
        case ARGS__TYPE_ENUM_STR:
            for (size_t i = 0; i < option->value.enum_.length; i++) {
                if (strcasecmp(option->value.enum_.values[i], value) != 0) continue;

                if (option->type == ARGS__TYPE_ENUM_IDX) {
                    option->value.enum_.as.index = i;
                } else {
                    option->value.enum_.as.value = option->value.enum_.values[i];
                }
                return;
            }
            ARGS__FATAL("Invalid value \"%s\" for option \"%s\"", value, option->long_name);
    }
}

ARGS__MAYBE_UNUSED static void args__print_str_default(FILE *fp, const char *str) {
    ARGS__ASSERT(fp != NULL);

    if (str == NULL) {
        fprintf(fp, "none");
        return;
    }

    fprintf(fp, "\"");
    for (const char *c = str; *c != '\0'; c++) {
        switch (*c) {
            case '\n': fprintf(fp, "\\n"); break;
            case '\r': fprintf(fp, "\\r"); break;
            case '\t': fprintf(fp, "\\t"); break;
            default:
                if (isprint(*c)) {
                    fprintf(fp, "%c", *c);
                } else {
                    fprintf(fp, "\\x%02hhx", *c);
                }
                break;
        }
    }
    fprintf(fp, "\"");
}

#ifndef ARGS_DISABLE_COMPLETION
// Prints strings with '\' and chars in `escaped_chars` escaped with '\'.
// Replaces '\n' with ' '.
static void args__completion_print_escaped(const char *str, const char *escaped_chars) {
    ARGS__ASSERT(str != NULL && escaped_chars != NULL);
    for (const char *c = str; *c != '\0'; c++) {
        if (*c == '\n') {
            printf(" ");
        } else {
            if (strchr(escaped_chars, *c) != NULL || *c == '\\') printf("\\");
            printf("%c", *c);
        }
    }
}

static void args__completion_bash_print(const char *program_name) {
    ARGS__ASSERT(program_name != NULL);
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
        program_name, program_name, program_name, program_name
    );
}

static void args__completion_zsh_print(const char *program_name) {
    ARGS__ASSERT(program_name != NULL);
    printf(
        "#compdef %s\n"
        "\n"
        "_%s() {\n"
        "    local IFS=$'\\n'\n"
        "    _arguments $(%s __complete zsh 2>/dev/null)\n"
        "}\n"
        "\n"
        "_%s\n",
        program_name, program_name, program_name, program_name
    );
}

static void args__completion_fish_print(const char *program_name) {
    ARGS__ASSERT(program_name != NULL);
    printf(
        "complete -c %s -e\n"
        "\n"
        "for args in (%s __complete fish 2>/dev/null)\n"
        "    eval \"complete -c %s $args\"\n"
        "end\n",
        program_name, program_name, program_name
    );
}

static void args__completion_bash_complete(Args *a) {
    ARGS__ASSERT(a != NULL);
    for (Args__Option *i = a->head; i != NULL; i = i->next) {
        if (i->short_name != '\0') printf("-%c ", i->short_name);
        printf("--%s ", i->long_name);
    }
    printf("\n");
}

static void args__completion_zsh_print_option_details(Args__Option *option) {
    ARGS__ASSERT(option != NULL);

    if (option->description != NULL) {
        printf("[");
        args__completion_print_escaped(option->description, "]");
        printf("]");
    }

    switch (option->type) {
        case ARGS__TYPE_PATH: printf(":path:_files"); break;
        case ARGS__TYPE_ENUM_IDX:
        case ARGS__TYPE_ENUM_STR:
            printf(":%s:(", option->long_name);
            for (size_t i = 0; i < option->value.enum_.length; i++) {
                if (i > 0) printf(" ");
                args__completion_print_escaped(option->value.enum_.values[i], " :()");
            }
            printf(")");
            break;
        default: break;  // Other types can accept anything, no suggestions.
    }
    printf("\n");
}

static void args__completion_zsh_complete(Args *a) {
    ARGS__ASSERT(a != NULL);
    for (Args__Option *i = a->head; i != NULL; i = i->next) {
        if (i->short_name != '\0') {
            printf("(-%c --%s)-%c", i->short_name, i->long_name, i->short_name);
            args__completion_zsh_print_option_details(i);

            printf("(-%c --%s)", i->short_name, i->long_name);
        }
        printf("--%s", i->long_name);
        if (i->type != ARGS__TYPE_BOOL) printf("=");
        args__completion_zsh_print_option_details(i);
    }
    // Set default completion of positional arguments to path.
    printf("*:file:_files\n");
}

static void args__completion_fish_complete(Args *a) {
    ARGS__ASSERT(a != NULL);
    for (Args__Option *i = a->head; i != NULL; i = i->next) {
        printf("-l %s -%c", i->long_name, i->type == ARGS__TYPE_PATH ? 'F' : 'f');
        if (i->type == ARGS__TYPE_ENUM_IDX || i->type == ARGS__TYPE_ENUM_STR) {
            printf(" -a '");
            for (size_t j = 0; j < i->value.enum_.length; j++) {
                if (j > 0) printf(" ");
                args__completion_print_escaped(i->value.enum_.values[j], " \"$()");
            }
            printf("'");
        }
        if (i->short_name != '\0') {
            // Use '-r' to avoid stacking short options.
            printf(" -s %c -r", i->short_name);
        }
        if (i->description != NULL) {
            printf(" -d \"");
            args__completion_print_escaped(i->description, "\"$");
            printf("\"");
        }
        printf("\n");
    }
}
#endif

// ====================================================================================================================
//                                                    PUBLIC API
// ====================================================================================================================

// Frees all the memory, including option values and array of positional arguments.
static void free_args(Args *a) {
    if (a == NULL) return;

    Args__Option *current = a->head;
    while (current != NULL) {
        switch (current->type) {
            case ARGS__TYPE_LONG:
            case ARGS__TYPE_FLOAT:
            case ARGS__TYPE_BOOL:  break;
            case ARGS__TYPE_STR:
            case ARGS__TYPE_PATH:
                free(current->default_value.str);
                free(current->value.str);
                break;
            case ARGS__TYPE_ENUM_STR:
            case ARGS__TYPE_ENUM_IDX:
                free(current->default_value.enum_);
                for (size_t i = 0; i < current->value.enum_.length; i++) free(current->value.enum_.values[i]);
                free(current->value.enum_.values);
                break;
        }
        free(current->long_name);
        free(current->description);

        Args__Option *next = current->next;
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
ARGS__MAYBE_UNUSED ARGS__WARN_UNUSED_RESULT static long *option_long(
    Args *a,
    char short_name,
    const char *long_name,
    const char *description,
    bool is_optional,
    long default_value
) {
    ARGS__ASSERT(a != NULL);
    Args__Option *option = args__new_option(a, short_name, long_name, description, is_optional, ARGS__TYPE_LONG);
    option->default_value.long_ = option->value.long_ = default_value;
    return &option->value.long_;
}

// Defines a float option, returns a pointer set by `parse_args`.
// Use '\0' for no short name.
// Exits if `a` or `long_name` is NULL, or out of memory.
ARGS__MAYBE_UNUSED ARGS__WARN_UNUSED_RESULT static float *option_float(
    Args *a,
    char short_name,
    const char *long_name,
    const char *description,
    bool is_optional,
    float default_value
) {
    ARGS__ASSERT(a != NULL);
    Args__Option *option = args__new_option(a, short_name, long_name, description, is_optional, ARGS__TYPE_FLOAT);
    option->default_value.float_ = option->value.float_ = default_value;
    return &option->value.float_;
}

// Defines a string option, returns a pointer set by `parse_args`.
// String memory is owned by library, freed by `free_args`.
// Result can be NULL only if default value is NULL.
// Use '\0' for no short name.
// Exits if `a` or `long_name` is NULL, or out of memory.
ARGS__MAYBE_UNUSED ARGS__WARN_UNUSED_RESULT static const char **option_str(
    Args *a,
    char short_name,
    const char *long_name,
    const char *description,
    bool is_optional,
    const char *default_value
) {
    ARGS__ASSERT(a != NULL);
    Args__Option *option = args__new_option(a, short_name, long_name, description, is_optional, ARGS__TYPE_STR);
    if (default_value == NULL) {
        option->default_value.str = NULL;
        option->value.str = NULL;
    } else {
        option->default_value.str = args__strdup(default_value);
        option->value.str = args__strdup(default_value);
    }
    return (const char **) &option->value.str;
}

// Same as `option_str` except that shell completion will suggest paths.
// Does NOT check that the value is a path.
ARGS__MAYBE_UNUSED ARGS__WARN_UNUSED_RESULT static const char **option_path(
    Args *a,
    char short_name,
    const char *long_name,
    const char *description,
    bool is_optional,
    const char *default_value
) {
    ARGS__ASSERT(a != NULL);
    Args__Option *option = args__new_option(a, short_name, long_name, description, is_optional, ARGS__TYPE_PATH);
    if (default_value == NULL) {
        option->default_value.str = NULL;
        option->value.str = NULL;
    } else {
        option->default_value.str = args__strdup(default_value);
        option->value.str = args__strdup(default_value);
    }
    return (const char **) &option->value.str;
}

// Defines a boolean flag, returns a pointer set by `parse_args`.
// Use '\0' for no short name.
// Exits if `a` or `long_name` is NULL, or out of memory.
ARGS__MAYBE_UNUSED ARGS__WARN_UNUSED_RESULT static bool *option_flag(
    Args *a,
    char short_name,
    const char *long_name,
    const char *description
) {
    ARGS__ASSERT(a != NULL);
    Args__Option *option = args__new_option(a, short_name, long_name, description, true, ARGS__TYPE_BOOL);
    option->value.bool_ = false;
    return &option->value.bool_;
}

// Defines an enum option, returns a pointer set by `parse_args`.
// Result is either a valid index or a default value.
// `values` must be a NULL-terminated array, matched case-insensitively.
// Use '\0' for no short name.
// Exits if `a`, `long_name`, or `values` is NULL, or out of memory.
ARGS__MAYBE_UNUSED ARGS__WARN_UNUSED_RESULT static size_t *option_enum(
    Args *a,
    char short_name,
    const char *long_name,
    const char *description,
    bool is_optional,
    size_t default_value,
    const char **values
) {
    ARGS__ASSERT(a != NULL && values != NULL);
    Args__Option *option = args__new_option(a, short_name, long_name, description, is_optional, ARGS__TYPE_ENUM_IDX);
    args__set_enum_values(option, values);
    option->value.enum_.as.index = default_value;
    if (default_value < option->value.enum_.length) {
        option->default_value.enum_ = args__strdup(values[default_value]);
    } else {
        option->default_value.enum_ = NULL;
    }
    return &option->value.enum_.as.index;
}

// Defines an enum option, returns a pointer set by `parse_args`.
// Result is either a string from `values` or a default value.
// String and array memory is owned by library, freed by `free_args`.
// `values` must be a NULL-terminated array, matched case-insensitively.
// Use '\0' for no short name.
// Exits if `a`, `long_name`, or `values` is NULL, or out of memory.
ARGS__MAYBE_UNUSED ARGS__WARN_UNUSED_RESULT static const char **option_enum_str(
    Args *a,
    char short_name,
    const char *long_name,
    const char *description,
    bool is_optional,
    const char *default_value,
    const char **values
) {
    ARGS__ASSERT(a != NULL && values != NULL);
    Args__Option *option = args__new_option(a, short_name, long_name, description, is_optional, ARGS__TYPE_ENUM_STR);
    option->default_value.enum_ = default_value == NULL ? NULL : args__strdup(default_value);
    args__set_enum_values(option, values);
    option->value.enum_.as.value = option->default_value.enum_;
    return &option->value.enum_.as.value;
}

// Parses arguments, sets option-returned values.
// Unless disabled, handles shell completion by writing to stdout and exiting.
// Must be called before side effects or stdout output.
// Returns positional arguments via `pos_args`, and their count as return value.
// Elements are from `argv`, while the array memory is managed by library.
// On error, prints to stderr and exits.
static int parse_args(Args *a, int argc, char **argv, char ***pos_args) {
    ARGS__ASSERT(a != NULL && argv != NULL);

    ARGS__ASSERT(argc >= 0);
    if (argc == 0) ARGS__FATAL("Expected the first argument to be a program name");
    ARGS__MAYBE_UNUSED const char *program_name = args__basename(argv[0]);
    argc--;
    argv++;

    for (Args__Option *i = a->head; i != NULL; i = i->next) {
        for (Args__Option *j = i->next; j != NULL; j = j->next) {
            if (i->short_name == j->short_name && i->short_name != '\0') {
                ARGS__FATAL(
                    "Duplicate short name '%c' in options \"%s\" and \"%s\"",  //
                    i->short_name, i->long_name, j->long_name
                );
            }

            if (strcmp(i->long_name, j->long_name) == 0) ARGS__FATAL("Duplicate option \"%s\"", i->long_name);
        }
    }

#ifndef ARGS_DISABLE_COMPLETION
    if (argc >= 1 && strcmp(argv[0], "completion") == 0) {
        if (argc == 1) ARGS__FATAL("Command 'completion' requires an argument: bash, zsh, fish");

        for (const char *c = program_name; *c != '\0'; c++) {
            if (isalnum(*c) || *c == '_' || *c == '.' || *c == '+' || *c == ':') continue;
            if (*c == '-' && c != program_name) continue;
            ARGS__FATAL("Invalid program name \"%s\"", program_name);
        }

        if (strcmp(argv[1], "bash") == 0) {
            args__completion_bash_print(program_name);
        } else if (strcmp(argv[1], "zsh") == 0) {
            args__completion_zsh_print(program_name);
        } else if (strcmp(argv[1], "fish") == 0) {
            args__completion_fish_print(program_name);
        } else {
            ARGS__FATAL("Failed to generate completion script: unknown shell \"%s\"", argv[1]);
        }
        free_args(a);
        exit(EXIT_SUCCESS);
    }

    if (argc >= 1 && strcmp(argv[0], "__complete") == 0) {
        if (argc == 1) ARGS__FATAL("Command '__complete' requires an argument: bash, zsh, fish");

        if (strcmp(argv[1], "bash") == 0) {
            args__completion_bash_complete(a);
        } else if (strcmp(argv[1], "zsh") == 0) {
            args__completion_zsh_complete(a);
        } else if (strcmp(argv[1], "fish") == 0) {
            args__completion_fish_complete(a);
        } else {
            ARGS__FATAL("Failed to generate completions: unknown shell \"%s\"", argv[1]);
        }
        free_args(a);
        exit(EXIT_SUCCESS);
    }
#endif

    int pos_args_idx = 0;
    if (pos_args != NULL) {
        a->pos_args = (char **) malloc(sizeof(*argv) * argc);
        if (a->pos_args == NULL) ARGS__OUT_OF_MEMORY();
        *pos_args = a->pos_args;
    }

    while (argc > 0) {
        char *arg = *argv;
        size_t arg_len = strlen(arg);
        argc--;
        argv++;

        if (arg_len < 2 || arg[0] != '-') {
            if (pos_args != NULL) (*pos_args)[pos_args_idx] = arg;
            pos_args_idx++;
            continue;
        }

        if (arg[1] == '-') {
            arg += 2;
            arg_len -= 2;

            Args__Option *option = a->head;
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
            if (option == NULL) ARGS__FATAL("Unknown or invalid option \"%s\"", arg);
#endif

#ifndef ARGS_ALLOW_OVERWRITING
            if (option->is_set) ARGS__FATAL("Option \"%s\" is set more than once", option->long_name);
#endif
            option->is_set = true;

            if (option->type == ARGS__TYPE_BOOL) {
                if (arg[option_len] == '=') ARGS__FATAL("Flags cannot have a value: \"%s\"", arg);
                option->value.bool_ = true;
                continue;
            }

            const char *value;
            if (arg[option_len] == '=') {
                value = arg + option_len + 1;
            } else {
                if (argc == 0) ARGS__FATAL("Option \"%s\" is missing a value", option->long_name);
                value = *argv;
                argc--;
                argv++;
            }

            args__parse_value(option, value);
        } else {
            if (arg_len != 2) ARGS__FATAL("Short option must be separate: \"%s\"", arg);

            arg += 1;
            arg_len -= 1;
            char ch = *arg;

            Args__Option *option = a->head;
            while (option != NULL && option->short_name != ch) option = option->next;

#ifdef ARGS_SKIP_UNKNOWN
            if (option == NULL) continue;
#else
            if (option == NULL) ARGS__FATAL("Unknown or invalid option \"%s\"", arg);
#endif

#ifndef ARGS_ALLOW_OVERWRITING
            if (option->is_set) ARGS__FATAL("Option '%c' is set more than once", option->short_name);
#endif
            option->is_set = true;

            if (option->type == ARGS__TYPE_BOOL) {
                option->value.bool_ = true;
                continue;
            }

            if (argc == 0) ARGS__FATAL("Option '%c' is missing a value", option->short_name);
            const char *value = *argv;
            argc--;
            argv++;

            args__parse_value(option, value);
        }
    }

    for (Args__Option *option = a->head; option != NULL; option = option->next) {
        if (!option->is_optional && !option->is_set) ARGS__FATAL("Missing a required option \"%s\"", option->long_name);
    }

    return pos_args_idx;
}

// Prints all options to `fp`.
// Caller is responsible for printing usage, as well as `completion` command.
ARGS__MAYBE_UNUSED static void print_options(Args *a, FILE *fp) {
    ARGS__ASSERT(a != NULL && fp != NULL);

    size_t longest_option = 0;
    for (Args__Option *option = a->head; option != NULL; option = option->next) {
        size_t length = strlen(option->long_name);
        if (length > longest_option) longest_option = length;
    }

    fprintf(fp, "Options:\n");
    for (Args__Option *option = a->head; option != NULL; option = option->next) {
        fprintf(fp, "  ");

        if (option->short_name != '\0') {
            fprintf(fp, "-%c, ", option->short_name);
        } else {
            fprintf(fp, "    ");
        }
        fprintf(fp, "--%s", option->long_name);

        int length_diff = longest_option - strlen(option->long_name);
        fprintf(fp, "%*c", length_diff + ARGS_PADDING, ' ');

        ARGS__MAYBE_UNUSED bool is_desc_multiline = false;
        int offset = 8 + longest_option + ARGS_PADDING;
        // Print description and break into multiple lines if needed.
        if (option->description != NULL) {
            int line_length = ARGS_LINE_LENGTH - offset;
            if (line_length < ARGS_MIN_DESC_LENGTH) line_length = ARGS_MIN_DESC_LENGTH;

            bool is_first = true;
            char *cur = option->description;
            int length = strlen(option->description);
            while (length > 0) {
                if (!is_first) {
                    is_desc_multiline = true;
                    fprintf(fp, "\n%*c", offset, ' ');
                }
                is_first = false;

                // Look for an explicit line break.
                int chunk_length = 0;
                while (chunk_length < length && chunk_length < line_length && cur[chunk_length] != '\n') chunk_length++;

                // If there isn't any, look for a space to break the line.
                if (chunk_length == line_length) {
                    while (chunk_length > 0 && cur[chunk_length] != ' ') chunk_length--;

                    // If failed to find a space or it is too early, break mid-word.
                    if (chunk_length < line_length / 2) chunk_length = line_length;
                }

                fprintf(fp, "%.*s", chunk_length, cur);

                if (cur[chunk_length] == ' ' || cur[chunk_length] == '\n') {
                    cur++;
                    length--;
                }
                cur += chunk_length;
                length -= chunk_length;
            }
        }

#ifndef ARGS_HIDE_DEFAULTS
        // Flag's default is always false, don't print their value.
        if (option->is_optional && option->type != ARGS__TYPE_BOOL) {
            if (is_desc_multiline) {
                // Print default on the new line to avoid breaking it too.
                fprintf(fp, "\n%*c", offset, ' ');
            } else {
                fprintf(fp, " ");
            }
            fprintf(fp, "(default: ");
            switch (option->type) {
                case ARGS__TYPE_LONG:     fprintf(fp, "%ld", option->default_value.long_); break;
                case ARGS__TYPE_FLOAT:    fprintf(fp, "%.3f", option->default_value.float_); break;
                case ARGS__TYPE_BOOL:     ARGS__UNREACHABLE();
                case ARGS__TYPE_STR:
                case ARGS__TYPE_PATH:     args__print_str_default(fp, option->default_value.str); break;
                case ARGS__TYPE_ENUM_STR:
                case ARGS__TYPE_ENUM_IDX: args__print_str_default(fp, option->default_value.enum_); break;
            }
            fprintf(fp, ")");
        }
#endif
        fprintf(fp, "\n");
    }
}

#undef ARGS__HAS_ATTRIBUTE
#undef ARGS__MAYBE_UNUSED
#undef ARGS__WARN_UNUSED_RESULT
#undef ARGS__FATAL
#undef ARGS__OUT_OF_MEMORY
#undef ARGS__UNREACHABLE
#undef ARGS__ASSERT
