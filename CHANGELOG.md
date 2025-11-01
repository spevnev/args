# Changelog

## Unreleased

### Added

- `ARGS_HIDE_DEFAULTS` to hide default values in `print_options`
- Tests for `print_options`
- Pre-commit hook to automatically update example in README

### Changed

- Improve formatting of long functions
- Handle escape sequences in default value of string option

### Fixed

- Fix namespaces
- Fix description splitting on non-spaces

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
