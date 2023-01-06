// Sample program demonstrating all features of 'globals' package.
#include <iostream>

//  Include globals.h in any file that will use global variables.
#include "globals.h"

int main(int argc, char** argv) {
  if (!set_parameter_file_arg_globals(argc, argv)) {
    exit(1);
  }
  set_command_line_globals(argc, argv);
  dump_globals();  // or: dump_globals("some_file.txt");
  f = 50;
  std::cout << "updated f + g = " << f + g << "\n";
}
