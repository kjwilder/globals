// globlist.h contains global variable definitions with optional defaults.
//
// There is no guard against multiple includes because this file needs to
// be included multiple times in globals.cc.
//
// This file does not include <string> as it is included from globals.h.
//
// This file should never be #included directly in user code.
//
// Limitation: string vectors cannot have strings with underscores. They
// will get converted to commas. Underscores are used here to separate
// values in a vector as commas cannot be used.

#define _ ,

GLOBAL(double, alpha1)
GLOBAL(double, alpha2)
GLOBAL(int, dum1)
GLOBAL(int, dum2)
GLOBALA(string, names, {"abc" _ "def" _ "ghi"})
GLOBALA(int, vals, {1 _ 2 _ 3 _ 0 _ 0})
GLOBALA(double, doubles, {})

#undef _
