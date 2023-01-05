# Globals
A C++ library to create global variables and update them from the command line
or a parameter file.

## Notes
This library works well and may be useful for users looking for an easy way to
define global variables in a single file and conveniently update them from the
command line or a parameter file.  However, users may want to consider packages
such as Google's [gflags](https://github.com/gflags/gflags) that are better
supported and provide additional features.

## Getting started
- Add the files `globlist.h`, `globals.h` and `globals.cc` to a C++ project.
- Modify `globlist.h` to include global variable definitions as described
  in the `Examples` section below. Do not modify the other two files.
- Include `globals.h` in any file that needs access to global variables.
- Modify the C++ `main` function to call `set_parameter_file_globals` and
  `set_command_line_globals` as in this repo's `main.cc`.
- Compile and link `globals.cc` with the rest of your C++ code.

# Using parameter files to set global variables
Suppose your `globlist.h` contains the following variable definitions:
```
GLOBAL(int, iterations)
GLOBAL(double, stepsize, 0.1)
GLOBAL(string, title, "main")
GLOBALA(string, labels, {"x" _ "y"})
```
You can create a parameter file to set new values for these variables.
For example, create a file named `params` with the following lines:
```
# Overrides
iterations = 100
stepsize = 0.01
labels = {"first", "second", "third"}
```
Spacing is not important, and any line that begins with a '#' is a comment line
that is ignored.  If your executable is called `myprog`, you can run it as:
```
> myprog params
```
to set global variables with values indicated in `params`

## Usign the command line to set global variables
Supposed `globlist.h` is set for `myprog` as in the previous section.  Global
variables from the command line.
```
>  myprog iterations=20 labels='{"a", "b", "c"}' title="a b c"
```
Global variables also can be set from both a parameter file and the command line.
The parameter file must be the first argument, and any parameter file assignments
will get overridden by command line assignments.
```
>  myprog params iterations=20
```

## Debugging
There is a convenience function `dump_globals` that can print all the global
variables defined in `globlist.h` with their current values. It takes one
argument that can be a filename or have the special value "cout" in which
case the values are printed to cout (usually the terminal).

## Examples
- To add a single variable with a default initialization:
  ```
  GLOBAL(int, int1)
  GLOBAL(string, string1)
  ```
  The first statement creates a variable 'int1' with type int.
- To add a single variable and initialize it:
  ```
  GLOBALI(int, int2, 10)
  GLOBALI(string, string2, "myvalue")
  ```
- To add a vector and initialize it:
  ```
  GLOBALA(int, intvec, 4, 5 _ 6 _ 7 _ 8)
  GLOBALA(string, strvec, 3, "str1" _ "str2" _ "str3")
  ```
  The first example creates a vector of four integers named 'intvec' with the
  values 5, 6, 7 and 8.  It is not necessary to provide initial values for all
  of the array elements as leftover array elements will have a default
  initialization.
