#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <string>
#include <vector>

using std::string;

#ifndef GLOBALS_C_
  #define GLOBAL(x, y, z) extern x y;
  #define GLOBALV(x, y, z) extern std::vector<x> y;
#endif

#include "globlist.h"

#undef GLOBAL
#undef GLOBALV

int set_parameter_file_arg_globals(int argc, char** argv);
int set_parameter_file_globals(const string&  parfile);
void set_command_line_globals(int argc, char** argv);
void dump_globals(const string& dumpfile);

#endif  // GLOBALS_H_

