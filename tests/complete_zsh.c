#include "complete_bash.c"

//< __complete zsh
//> (-b --bool)-b[A bool option]
//> (-b --bool)--bool[A bool option]
//> --long=[A long option]
//> (-f --float)-f[A float option]
//> (-f --float)--float=[A float option]
//> --string=[A string option]
//> (-p --path)-p[A path option]:path:_files
//> (-p --path)--path=[A path option]:path:_files
//> --enum-idx=[An index enum option]:enum-idx:(first second third)
//> (-e --enum-str)-e[A string enum option]:enum-str:(first second third)
//> (-e --enum-str)--enum-str=[A string enum option]:enum-str:(first second third)
//> *:file:_files
