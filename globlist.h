// globlist.h contains global variable definitions with optional defaults.
//
// globlist.h does not #include <string> as <string> is included from
// globals.cc and globals.h, the only two files that should ever #include
// globlist.h.  There is no guard against multiple #includes because globlist.h
// needs to be #included multiple times in globals.cc.

GLOBAL(double, f)
GLOBAL(double, g)
GLOBAL(int, x)
GLOBAL(int, y)
GLOBALA(string, names, ({"abc" , "def" , "ghi"}))
GLOBALA(int, vals, ({1 , 2 , 3 , 0 , 0}))
GLOBALA(double, doubles, ({}))
