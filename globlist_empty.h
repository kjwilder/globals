// globlist.h - Global variable definitions.
//
// Copy this file to your project as "globlist.h" and add your global
// variables using the GLOBAL and GLOBALV macros:
//
//   GLOBAL(type, name, default_value)
//   GLOBALV(type, name, (initial_values))
//
// Supported types: int, long, double, string
//
// Examples:
//   GLOBAL(int, iterations, 100)
//   GLOBAL(double, threshold, 0.5)
//   GLOBAL(string, output_file, "out.txt")
//   GLOBALV(int, counts, ({1, 2, 3}))
//   GLOBALV(string, labels, ({"first", "second"}))
//   GLOBALV(double, weights, ({}))
//
// See also globlist_example.h for more examples.
