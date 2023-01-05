#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <string>
#include <vector>

using std::string;

#ifndef GLOBALS_C_
  #define GLOBAL(x, y) extern x y;
  #define GLOBALA(x, y, z) extern std::vector<x> y;
  #define GLOBALI(x, y, z) extern x y;
#endif

#include "globlist.h"

#undef GLOBAL
#undef GLOBALA
#undef GLOBALI

int set_parameter_file_globals(int argc, char** argv);
void set_command_line_globals(int argc, char** argv);
void dump_globals(const string& dumpfile);

#endif  // GLOBALS_H_
