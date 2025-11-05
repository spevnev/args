#include <cassert>
#include <cstdlib>
#include "args.h"

//< -l 1 pos --enum-str=third -b --float=2.3 args -p /path --str string -e SECOND
//> Options:
//>   -l, --long
//>   -f, --float
//>   -s, --str
//>   -p, --path
//>   -b, --bool
//>   -e, --enum
//>   -E, --enum-str
int main(int argc, char **argv) {
    const char *ENUM_VALUES[] = {"first", "second", "third", nullptr};

    ArgsCpp args;
    const auto &l = args.option_long('l', "long", nullptr, false);
    const auto &f = args.option_float('f', "float", nullptr, false);
    const auto &s = args.option_str('s', "str", nullptr, false);
    const auto &p = args.option_path('p', "path", nullptr, false);
    const auto &b = args.option_flag('b', "bool", nullptr);
    const auto &ei = args.option_enum('e', "enum", nullptr, false, -1, ENUM_VALUES);
    const auto &es = args.option_enum_str('E', "enum-str", nullptr, false, nullptr, ENUM_VALUES);

    char **pos_args;
    int pos_args_len = args.parse_args(argc, argv, pos_args);

    assert(l == 1);
    assert(f == 2.3F);
    assert(strcmp(s, "string") == 0);
    assert(strcmp(p, "/path") == 0);
    assert(b);
    assert(ei == 1);
    assert(strcmp(es, "third") == 0);

    assert(pos_args_len == 2);
    assert(strcmp(pos_args[0], "pos") == 0);
    assert(strcmp(pos_args[1], "args") == 0);

    args.print_options();

    return EXIT_SUCCESS;
}
