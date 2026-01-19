# Globals
A C++ library to create global variables and update them from the command line
and/or a parameter file.

## Notes
This library works well and may be useful for users looking for an easy way to
define global variables in a single file and conveniently update them from the
command line or a parameter file.  However, users should consider packages
such as Google's [gflags](https://github.com/gflags/gflags) that are better
supported and provide additional features.

## Recent Improvements

Recent updates have significantly improved the library's robustness and reliability:

**Bug Fixes:**
- Safe type conversions with proper error handling (no more silent failures on invalid input)
- Fixed buffer overflow in vector parsing when input is malformed
- Fixed integer overflow detection for values exceeding int32 range
- Fixed atomic vector updates (partial updates no longer occur on conversion errors)
- Improved empty line and comment handling in parameter files

**Error Handling:**
- Invalid numeric values (e.g., `"abc"` for an integer) now properly fail with clear error messages
- Partially valid numbers (e.g., `"123abc"`) are rejected instead of silently truncated
- Vector values must be enclosed in braces `{...}` or an error will be reported
- All conversion errors include context about which variable and value failed

**Testing:**
- Comprehensive test suite with 29 tests covering all major functionality
- Tests verify scalar and vector operations, error handling, and edge cases
- Run tests with: `./simple_test` (standalone) or `bazel test //:globals_test` (Google Test)

## Usage
- Add the files [globlist.h](globlist.h), [globals.h](globals.h) and
  [globals.cc](globals.cc) to a C++ project.
- Modify your copy  of [globlist.h](globlist.h) to include global variable
  definitions as described in the `Examples` section below. Do not modify the
  other two files.
- Include [globals.h](globals.h) in any source file that needs access to global
  variables.
- Modify your `main` function to call `set_parameter_file_arg_globals` and
  `set_command_line_globals` as in this repo's `main.cc`.
- Compile [globals.cc](globals.cc) with the rest of your C++ code.


## Examples
- Add global variables to [globlist.h](globlist.h):
  ```
  GLOBAL(int, a, 0)
  GLOBAL(string, b, "")
  ```
  The first GLOBAL creates a global variable `a` with type `int` equal to `0`.
- Add global vectors:
  ```
  GLOBALV(int, x, ({5, 6, 7, 8}))
  GLOBALV(string, y, ({"str1", "str2", "str3"}))
  GLOBALV(double, z, ({}))
  ```
  The first example creates a vector named 'x' with entries 5, 6, 7 and 8.
  The array initializer argument must be surrounded by parentheses.
  
## Using parameter files to set global variables
Suppose your [globlist.h](globlist.h) contains the following variable
definitions:
```
GLOBAL(int, iterations,  100)
GLOBAL(double, stepsize, 0.1)
GLOBAL(string, title, "main")
GLOBALV(string, labels, ({"x", "y"}))
```
You can create a parameter file to set new values for these variables.
For example, create a file named `params.txt` with the following lines:
```
# Overrides
iterations = 100
stepsize = 0.01
# Do not include quotes in string assignments unless you want the string
# to have a quote.
labels = {first, second, third}
```
Spacing is not important, and any line that begins with a '#' is a comment line
that is ignored.  If your executable is called `myprog`, you can run it as:
```
> myprog params.txt
```
to update global variables with values provided in `params.txt`

## Using the command line to set global variables
Supposed your [globlist.h](globlist.h) is set for `myprog` as in the previous
section.  Global variables also can be specified from the command line.
```
>  myprog iterations=20 labels='{a, b, c}' title="a b c"
```
One can also use both a parameter file and the command line.  The parameter
file must be the first argument, and command line assignments will override
parameter file assignments.
```
>  myprog params.txt iterations=20
```

## Debugging
There is a convenience function `dump_globals` that can print all the global
variables defined in [globlist.h](globlist.h) with their current values. It
takes one argument that can be a filename or be left empty in which case the
output will be displayed on the terminal.

## Building

The library can be built using either Make or Bazel:

**Using Make:**
```bash
make program
./program [params.txt] [var=value ...]
```

**Using Bazel:**
```bash
bazel build //:program
bazel run //:program -- [params.txt] [var=value ...]
```

## Testing

The library includes comprehensive tests to ensure correctness and reliability.

**Run standalone tests (no dependencies required):**
```bash
g++ -Wall -std=c++11 globals.cc simple_test.cc -o simple_test
./simple_test
```

**Run Google Test suite (requires Bazel with Google Test):**
```bash
bazel test //:globals_test
```

The test suite includes 29 tests with 67 assertions covering:
- Scalar variable assignments (int, long, double, string)
- Vector operations (empty, single element, multiple elements)
- Negative numbers and scientific notation
- Invalid input detection and error handling
- Parameter file parsing with comments
- Command line argument processing
- Edge cases and boundary conditions

All tests validate that invalid inputs are properly rejected without corrupting the global state.

## Error Handling

The library now provides robust error handling:

- **Type Conversion Errors**: Invalid numeric strings are detected and reported with specific error messages
- **Format Validation**: Vector values must be enclosed in braces `{...}`
- **Overflow Detection**: Integer values exceeding the valid range are rejected
- **Atomic Updates**: Vectors are only updated if all elements convert successfully
- **Unknown Variables**: Warnings are printed but execution continues

Example error output:
```
Error converting value 'abc123': Invalid integer format
Error setting variable [x]: Invalid integer format
Unknown variable [x] on command line.
```
