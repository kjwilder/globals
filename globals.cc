#define GLOBALS_C_

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <climits>
#include <utility>

using std::cerr;
using std::string;
using std::vector;

#include "globals.h"

// __________________________________________________________________________
// Types and lists needed to keep track of global variables.

enum Types { intvar, longvar, doublevar, stringvar };

struct vardescriptor { void *ptr; Types type; bool is_vector;};

std::unordered_map<string, vardescriptor> varmap = {
#define GLOBAL(x, y, z) {#y, {&y, x##var, false}},
#define GLOBALV(x, y, z) {#y, {&y, x##var, true}},
#include "globlist.h"
#undef GLOBAL
#undef GLOBALV
};

// __________________________________________________________________________
// Safe type conversion functions with error handling.

static int safe_atoi(const char* str) {
  errno = 0;
  char* endptr = nullptr;
  long val = std::strtol(str, &endptr, 10);

  if (errno == ERANGE || val > INT32_MAX || val < INT32_MIN) {
    throw std::out_of_range("Integer value out of range");
  }
  if (endptr == str || *endptr != '\0') {
    throw std::invalid_argument("Invalid integer format");
  }
  return static_cast<int>(val);
}

static long safe_atol(const char* str) {
  errno = 0;
  char* endptr = nullptr;
  long val = std::strtol(str, &endptr, 10);

  if (errno == ERANGE) {
    throw std::out_of_range("Long value out of range");
  }
  if (endptr == str || *endptr != '\0') {
    throw std::invalid_argument("Invalid long format");
  }
  return val;
}

static double safe_atof(const char* str) {
  errno = 0;
  char* endptr = nullptr;
  double val = std::strtod(str, &endptr);

  if (errno == ERANGE) {
    throw std::out_of_range("Double value out of range");
  }
  if (endptr == str || *endptr != '\0') {
    throw std::invalid_argument("Invalid double format");
  }
  return val;
}

// __________________________________________________________________________

static string strip(const string& s) {
  string stripped;
  auto first = s.find_first_not_of(" \t\n\r");
  if (first == string::npos) {
    stripped = s;
  } else {
    auto last =  s.find_last_not_of(" \t\n\r");
    stripped = s.substr(first,  last - first + 1);
  }
  return stripped;
}

// __________________________________________________________________________

vector<string> split(const string& text) {
  vector<string> splits;
  if (text.empty()) {
    return splits;
  }
  std::size_t start = 0;
  std::size_t end = text.find(',', start);
  while (end != string::npos) {
    auto token = strip(text.substr(start, end - start));
    if (!token.empty()) {
      splits.push_back(token);
    }
    start = end + 1;
    end = text.find(',', start);
  }
  auto last_token = strip(text.substr(start));
  if (!last_token.empty()) {
    splits.push_back(last_token);
  }
  return splits;
}

// __________________________________________________________________________

template<class T>
void print_value(std::ostream& os, const vardescriptor& td, T inst) {
  os << *static_cast<T*>(td.ptr);
}

template<class T, class F>
void fill_value(const vardescriptor& td, const string& val, T inst, F func) {
  try {
    *static_cast<T*>(td.ptr) = func(val.c_str());
  } catch (const std::exception& e) {
    cerr << "Error converting value '" << val << "': " << e.what() << "\n";
    throw;
  }
}

// __________________________________________________________________________

template<class T>
void print_vector_value(std::ostream& os, const T& val, Types t) {
  os << val;
}

template<>
void print_vector_value<string>(std::ostream& os, const string& val, Types t) {
    os << "\"" << val << "\"";
}

template<class T>
void print_vector(std::ostream& os, const vardescriptor& td, T inst) {
  const vector<T>& vec = *static_cast<vector<T>*>(td.ptr);
  os << "{";
  if (vec.size() > 0) {
    for (size_t i = 0; i < vec.size() - 1; ++i) {
      print_vector_value(os, vec[i], td.type);
      os << ", ";
    }
    print_vector_value(os, vec[vec.size() - 1], td.type);
  }
  os << "}";
}

template<class T, class F>
void fill_vector(const vardescriptor& td, const string& val, T inst, F func) {
  string stripped = strip(val);

  // Check for braces and extract content
  if (stripped.size() < 2 || stripped[0] != '{' ||
      stripped[stripped.size() - 1] != '}') {
    cerr << "Vector value must be enclosed in braces: " << val << "\n";
    throw std::invalid_argument("Vector value must be enclosed in braces");
  }

  string content = stripped.substr(1, stripped.size() - 2);
  const vector<string> values = split(content);

  // Convert all values to a temporary vector first to avoid partial updates
  vector<T> temp_vec;
  temp_vec.reserve(values.size());

  try {
    for (size_t i = 0; i < values.size(); ++i) {
      temp_vec.push_back(func(values[i].c_str()));
    }
  } catch (const std::exception& e) {
    cerr << "Error converting vector element: " << e.what() << "\n";
    throw;
  }

  // Only update the actual vector if all conversions succeeded
  vector<T>* ptr = static_cast<vector<T>*>(td.ptr);
  *ptr = std::move(temp_vec);
}

// __________________________________________________________________________
// Set the global variable 'var' equal to 'val' (after casting)

int set_global(const string& var, const string& val) {
  if (varmap.find(var) == varmap.end()) {
    cerr << "Unable to determine type of variable [" << var << "].\n";
    return 0;
  }

  try {
    if (!varmap[var].is_vector) {
      switch (varmap[var].type) {
        case (intvar):
          fill_value(varmap[var], val, int(), safe_atoi);
          break;
        case (longvar):
          fill_value(varmap[var], val, int64_t(), safe_atol);
          break;
        case (doublevar):
          fill_value(varmap[var], val, double(), safe_atof);
          break;
        case (stringvar):
          *static_cast<string*>(varmap[var].ptr) = val;
          break;
      }
    } else {
      switch (varmap[var].type) {
        case (intvar):
          fill_vector(varmap[var], val, int(), safe_atoi);
          break;
        case (longvar):
          fill_vector(varmap[var], val, int64_t(), safe_atol);
          break;
        case (doublevar):
          fill_vector(varmap[var], val, double(), safe_atof);
          break;
        case (stringvar):
          {
            string stripped = strip(val);
            if (stripped.size() < 2 || stripped[0] != '{' ||
                stripped[stripped.size() - 1] != '}') {
              cerr << "Vector value must be enclosed in braces: " << val << "\n";
              throw std::invalid_argument("Vector value must be enclosed in braces");
            }
            string content = stripped.substr(1, stripped.size() - 2);
            const auto ptr = static_cast<vector<string>*>(varmap[var].ptr);
            ptr->clear();
            for (const auto& w : split(content)) {
              ptr->push_back(w);
            }
          }
          break;
      }
    }
  } catch (const std::exception& e) {
    cerr << "Error setting variable [" << var << "]: " << e.what() << "\n";
    return 0;
  }
  return 1;
}

// __________________________________________________________________________
// For an 'eqn' of the form ' A B C = D E F ', make 'var' equal to "C" and
// 'val' equal to "D E F".

void parse_equation(const string& eqn, string* var, string* val) {
  var->clear();
  val->clear();

  auto eq_pos = eqn.find('=');
  if (eq_pos == string::npos) {
    return;
  }

  *var = strip(eqn.substr(0, eq_pos));
  *val = strip(eqn.substr(eq_pos + 1, eqn.size() - eq_pos));

  // Check for empty var or comments
  if (var->empty() || (*var)[0] == '#') {
    var->clear();
    val->clear();
    return;
  }

  // If 'var' contains multiple tokens, keep only the last one.
  auto sp_pos = var->find_last_of(" \t\n\r");
  if (sp_pos != string::npos) {
    *var = var->substr(sp_pos + 1, var->size() - sp_pos);
  }
}

// __________________________________________________________________________
// Parse assignments of the form var=value to set global variables.

void set_command_line_globals(int argc, char** argv) {
  // Skip argv[1] if it is the name of a parameter file
  int count = (argc > 1 && strchr(argv[1], '=') == 0) ? 2 : 1;

  // Parse the remaining arguments
  while (count < argc) {
    string var, val;
    parse_equation(argv[count++], &var, &val);
    if (!var.empty() && !set_global(var, val)) {
      cerr << "Unknown variable [" << var << "] on command line.\n";
    }
  }
}

// __________________________________________________________________________

int set_parameter_file_arg_globals(int argc, char** argv) {
  // See if a parameter file was specified as the first command line arg.
  string parfile;
  if (argc > 1 && strchr(argv[1], '=') == 0) {
    parfile = argv[1];
  } else {
    return 1;
  }
  return set_parameter_file_globals(parfile);
}

// __________________________________________________________________________
// This can be called at anytime to update globals using assignments
// from a parameter file 'parfile'.

int set_parameter_file_globals(const string&  parfile) {
  std::ifstream ifs(parfile);
  if (!ifs) {
    std::cerr << "Unable to open parameter file [" << parfile << "].\n";
    return 0;
  }

  string buf, var, val;
  while (std::getline(ifs, buf)) {
    parse_equation(buf, &var, &val);
    if (!var.empty() && !set_global(var, val)) {
      // Warn but don't fail on unknown variables.
      cerr << "Unknown variable [" << var << "] in parameter file.\n";
    }
  }
  return 1;
}

// __________________________________________________________________________
// Print out all global variables to stdout or a file.

void dump_globals(const string& dump_file) {
  std::ofstream ofs;
  if (!dump_file.empty()) {
    ofs.open(dump_file);
    if (!ofs) {
      cerr << "Unable to open dump_file [" << dump_file << "].\n";
      return;
    }
  }
  std::ostream& os = dump_file.empty() ? std::cout : ofs;

  for (const auto& vl : varmap) {
    os << vl.first << " = ";
    if (vl.second.is_vector) {
      switch (vl.second.type) {
        case (intvar):
          print_vector(os, vl.second, int());
          break;
        case (longvar):
          print_vector(os, vl.second, int64_t());
          break;
        case (doublevar):
          print_vector(os, vl.second, double());
          break;
        case (stringvar):
          print_vector(os, vl.second, string());
          break;
      }
    } else {
      switch (vl.second.type) {
        case (intvar):
          print_value(os, vl.second, int());
          break;
        case (longvar):
          print_value(os, vl.second, int64_t());
          break;
        case (doublevar):
          print_value(os, vl.second, double());
          break;
        case (stringvar):
          os << "\"";
          print_value(os, vl.second, string());
          os << "\"";
          break;
      }
    }
    os << "\n";
  }
}
