#include <cassert>
#include <cstdlib>
#include "args.h"

//< -l 1 pos --enum-str=third -b --float=2.3 args -p path --str string -e SECOND
//> Options:
//>   -h, --help      Show help
//>   -v, --version   Print version
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
    args.option_help([](ArgsCpp &, const char *) { printf("help message\n"); });
    args.option_version("current version: 1.2.3");
    const auto &l = args.option_long("long", nullptr).short_name('l').required();
    const auto &f = args.option_float("float", nullptr).short_name('f').required();
    const auto &s = args.option_string("str", nullptr).short_name('s').required();
    const auto &p = args.option_path("path", nullptr).short_name('p').required();
    const auto &b = args.option_flag("bool", nullptr).short_name('b');
    const auto &ei = args.option_enum("enum", nullptr, ENUM_VALUES).short_name('e').required();
    const auto &es = args.option_enum_string("enum-str", nullptr, ENUM_VALUES).short_name('E').required();

    char **pos_args;
    int pos_args_len = args.parse_args(argc, argv, pos_args);

    assert(l == 1);
    assert(f == 2.3F);
    assert(strcmp(s, "string") == 0);
    assert(strcmp(p, "path") == 0);
    assert(b);
    assert(ei == 1);
    assert(strcmp(es, "third") == 0);

    assert(pos_args_len == 2);
    assert(strcmp(pos_args[0], "pos") == 0);
    assert(strcmp(pos_args[1], "args") == 0);

    args.print_options();

    return EXIT_SUCCESS;
}
