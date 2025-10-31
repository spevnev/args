#define ARGS_LINE_LENGTH 200
#include "print.c"

// clang-format off
//< -f 1
//> Options:
//>   -b, --bool                   Option with a description so long that it must be split to the second line (default: false)
//>       --option-with-long-name  A long option (default: 12)
//>   -f, --float                  A float option
//>       --str                    A string option (default: "some default")
//>   -p, --path                   A path option (default: none)
// clang-format on
