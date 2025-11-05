#define ARGS_PADDING 10
#include "print.c"

//< -f 1
//> Options:
//>   -b, --bool                           Option with a description so long that it
//>                                        must be split to the second line
//>       --option-with-long-name          A long option (default: 12)
//>   -f, --float                          A float option
//>       --str                            A string option (default: "some default")
//>   -p, --path                           A path option (default: none)
//>       --enum-idx                       An index enum option (default: none)
//>   -e, --enum-str                       A string enum option (default: "enum default")
