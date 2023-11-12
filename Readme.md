# A simple flag library in c.

Flag library which intentionally ignores invalid flags in most cases (I wrote this for dic since I wanted to be able to pass args starting with "-").

### Specifics:

- Unknown two-dash flags are ignored.
- If the first character is a one-dash flag is not a known flag, the entire argument is ignored.
- Each argument is marked in a boolean array for the caller to know if it was parsed or ignored.
- If any error occurs on an argument that was recognized as a flag (or it's argument), it is reported to stderr and true is returned.

## Usage

Only one function `flag_ignore`, is provided. To use it you need to give it arrays of the 4 structs provided:

- `BoolOneFlag` Boolean receiving, one-dash flag.
- `BoolTwoFlag` Boolean receiving, two-dash flag.
- `ArgOneFlag` Arg receiving, one-dash flags.
- `ArgTwoFlag` Arg receiving, two-dash flags.

Each struct contains a value to indicating the what the flag is, and a pointer to modify if the flag occurred: either a boolean or a string for the arg after the flag.

Each array is terminated with the flag value: '\0' for one-flags, and NULL for two-flags.

See further documentation in the header file.

## License

This program is licensed under GPLv3.
