#define ARGS_LINE_LENGTH 20
#define ARGS_MIN_DESC_LENGTH 60
#include "print.c"

//< -f 1
//> Options:
//>   -b, --bool                   Option with a description so long that it must be split to
//>                                the second line
//>                                (default: false)
//>       --option-with-long-name  A long option (default: 12)
//>   -f, --float                  A float option
//>       --str                    A string option (default: some default)
//>   -p, --path                   A path option (default: none)
