# Globals
A C++ library to create global variables and update them from the command line
and/or a parameter file.

## Notes
This library works well and may be useful for users looking for an easy way to
define global variables in a single file and conveniently update them from the
command line or a parameter file.  However, users should consider packages
such as Google's [gflags](https://github.com/gflags/gflags) that are better
supported and provide additional features.

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
- Add variables with default values to your copy of [globlist.h](globlist.h):
  ```
  GLOBAL(int, a)
  GLOBAL(string, b)
  ```
  The first GLOBAL creates a global variable `a` with type `int` equal to `0`.
- Add variables with non-default values:
  ```
  GLOBALI(int, c, 10)
  GLOBALI(string, d, "myvalue")
  ```
- Add vectors:
  ```
  GLOBALA(int, x, ({5, 6, 7, 8}))
  GLOBALA(string, y, ({"str1", "str2", "str3"}))
  GLOBALA(double, z, ({}))
  ```
  The first example creates a vector named 'x' with entries 5, 6, 7 and 8.
  The array initializer argument must be surrounded by parentheses.
  
## Using parameter files to set global variables
Suppose your [globlist.h](globlist.h) contains the following variable
definitions:
```
GLOBAL(int, iterations)
GLOBAL(double, stepsize, 0.1)
GLOBAL(string, title, "main")
GLOBALA(string, labels, ({"x", "y"}))
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

## Usign the command line to set global variables
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
takes one argument that can be a filename or have the special value "cout" in
which case the values are printed to cout (usually the terminal).
