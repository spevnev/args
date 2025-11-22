Features:
- Subcommands
- Repeated options (collect all occurrences into a list)
- Option groups
- Mutex & required groups
- .needs & .conflicts named arguments
- Custom completions?
- Hide, show, and custom default?
- UTF8 support?

Improvements:
- Bash: don't suggest options that are already set (check `$words`)
- More info in `print_options` (e.g. required, possible enum values)?
