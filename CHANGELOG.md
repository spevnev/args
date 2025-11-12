# Changelog

## Unreleased

### **BREAKING** Changes

- Completely change option functions: turn `short_name`, `is_optional`, and `default_value` into optional arguments
- Make options return constant pointers
- Rename `ARGS_MIN_DESC_LENGTH` to `ARGS_MIN_DESCRIPTION_LENGTH`
- Rename `option_str` and `option_enum_str` to `option_string` and `option_enum_string`
- Reorder `option_enum` arguments

### Added

- `ignore_required` named argument for `option_flag`, intended for flags like `help` and `version`
- `hidden` to hide option from `print_options`, intended for deprecated or developer options

## [1.3.0](https://github.com/spevnev/args/releases/tag/v1.3.0)

### Added

- Allow specifying line breaks ('\n') in description
- C++ wrapper `ArgsCpp`

### Changed

- Make C++ compatible
- Improve bash completion: add descriptions, suggest values
- Allow stacked short options, e.g. `-a -b` -> `-ab`, `-s string` -> `-sstring`, `-v -l 2` -> `-vl2`.
- Don't print `option_flag`'s default since it's always false
- Add error on defining new options after `parse_args`

### Fixed

- Fix `__has_attribute` check
- Fix `print_options` printing current value instead of default
- Fix `print_options` printing trailing whitespace after option with no default and no description
- Fix `print_options` printing whitespaces before long option even when no option has a short name

## [1.2.0](https://github.com/spevnev/args/releases/tag/v1.2.0)

### Added

- `ARGS_HIDE_DEFAULTS` to hide default values in `print_options`
- Enum options: `option_enum` and `option_enum_str`
- Tests for `print_options`
- Pre-commit hook to automatically update example in README

### Changed

- Improve formatting of long functions
- Handle escape sequences in default value of string option

### Fixed

- Fix namespaces
- Fix description splitting on non-spaces
- Some tests missing `free_args`
- Return number of positional arguments from `parse_args` even when `pos_args` is NULL

## [1.1.0](https://github.com/spevnev/args/releases/tag/v1.1.0)

### Added

- Shell completion for bash, zsh, fish
- `ARGS_DISABLE_COMPLETION` option
- Warning on unused function result
- New option: `option_path` for shell completion
- Automated tests

### Changed

- Improve and explain example
- Check that long option and description are printable

### Fixed

- Silence unused function warning

## [1.0.0](https://github.com/spevnev/args/releases/tag/v1.0.0)

Initial release
