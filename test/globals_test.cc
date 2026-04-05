// Unit tests for the globals library.
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "globals.h"

using std::string;
using std::vector;

static int tests_run = 0;
static int tests_failed = 0;

#define EXPECT_EQ(a, b) \
  do { \
    ++tests_run; \
    auto _a = (a); \
    auto _b = (b); \
    if (_a != _b) { \
      std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ \
                << "  " #a " != " #b "\n"; \
      ++tests_failed; \
    } \
  } while (0)

// Reset all test globals to their initial defaults.
static void reset() {
  int_var    = 42;
  double_var = 1.5;
  string_var = "hello";
  int_vec    = {1, 2, 3};
  double_vec = {0.5, 1.5};
  string_vec = {"a", "b", "c"};
}

// Write content to a temp file and return its path.
static string write_tmpfile(const string& content) {
  const string path = "/tmp/globals_test_params.txt";
  std::ofstream f(path);
  f << content;
  return path;
}

// ---- Tests ------------------------------------------------------------------

static void test_defaults() {
  reset();
  EXPECT_EQ(int_var, 42);
  EXPECT_EQ(double_var, 1.5);
  EXPECT_EQ(string_var, string("hello"));
  EXPECT_EQ(int_vec.size(), 3u);
  EXPECT_EQ(int_vec[0], 1);
  EXPECT_EQ(int_vec[1], 2);
  EXPECT_EQ(int_vec[2], 3);
  EXPECT_EQ(double_vec.size(), 2u);
  EXPECT_EQ(double_vec[0], 0.5);
  EXPECT_EQ(string_vec.size(), 3u);
  EXPECT_EQ(string_vec[0], string("a"));
  EXPECT_EQ(string_vec[2], string("c"));
}

static void test_set_command_line_int() {
  reset();
  const char* argv[] = {"prog", "int_var=99"};
  set_command_line_globals(2, const_cast<char**>(argv));
  EXPECT_EQ(int_var, 99);
}

static void test_set_command_line_double() {
  reset();
  const char* argv[] = {"prog", "double_var=2.5"};
  set_command_line_globals(2, const_cast<char**>(argv));
  EXPECT_EQ(double_var, 2.5);
}

static void test_set_command_line_string() {
  reset();
  const char* argv[] = {"prog", "string_var=world"};
  set_command_line_globals(2, const_cast<char**>(argv));
  EXPECT_EQ(string_var, string("world"));
}

static void test_set_command_line_int_vector() {
  reset();
  const char* argv[] = {"prog", "int_vec={4,5,6,7}"};
  set_command_line_globals(2, const_cast<char**>(argv));
  EXPECT_EQ(int_vec.size(), 4u);
  EXPECT_EQ(int_vec[0], 4);
  EXPECT_EQ(int_vec[1], 5);
  EXPECT_EQ(int_vec[2], 6);
  EXPECT_EQ(int_vec[3], 7);
}

static void test_set_command_line_double_vector() {
  reset();
  const char* argv[] = {"prog", "double_vec={2.5,3.5,4.5}"};
  set_command_line_globals(2, const_cast<char**>(argv));
  EXPECT_EQ(double_vec.size(), 3u);
  EXPECT_EQ(double_vec[0], 2.5);
  EXPECT_EQ(double_vec[1], 3.5);
  EXPECT_EQ(double_vec[2], 4.5);
}

static void test_set_command_line_string_vector() {
  reset();
  const char* argv[] = {"prog", "string_vec={x,y}"};
  set_command_line_globals(2, const_cast<char**>(argv));
  EXPECT_EQ(string_vec.size(), 2u);
  EXPECT_EQ(string_vec[0], string("x"));
  EXPECT_EQ(string_vec[1], string("y"));
}

static void test_set_command_line_multiple() {
  reset();
  const char* argv[] = {"prog", "int_var=7", "string_var=bye"};
  set_command_line_globals(3, const_cast<char**>(argv));
  EXPECT_EQ(int_var, 7);
  EXPECT_EQ(string_var, string("bye"));
}

static void test_command_line_skips_first_arg_without_equals() {
  // argv[1] without '=' is treated as a filename and skipped.
  reset();
  const char* argv[] = {"prog", "notafile", "int_var=7"};
  set_command_line_globals(3, const_cast<char**>(argv));
  EXPECT_EQ(int_var, 7);
}

static void test_set_command_line_unknown_var() {
  // Unknown variable should not crash or affect known variables.
  reset();
  const char* argv[] = {"prog", "no_such_var=1", "int_var=5"};
  set_command_line_globals(3, const_cast<char**>(argv));
  EXPECT_EQ(int_var, 5);
}

static void test_set_parameter_file() {
  reset();
  const string path = write_tmpfile("int_var = 77\ndouble_var = 0.5\n");
  EXPECT_EQ(set_parameter_file_globals(path), 1);
  EXPECT_EQ(int_var, 77);
  EXPECT_EQ(double_var, 0.5);
  std::remove(path.c_str());
}

static void test_set_parameter_file_comments() {
  reset();
  const string path = write_tmpfile(
      "# this is a comment\n"
      "int_var = 55\n"
      "# another comment\n");
  set_parameter_file_globals(path);
  EXPECT_EQ(int_var, 55);
  EXPECT_EQ(double_var, 1.5);  // unchanged
  std::remove(path.c_str());
}

static void test_set_parameter_file_whitespace() {
  reset();
  const string path = write_tmpfile("  int_var  =  88  \n");
  set_parameter_file_globals(path);
  EXPECT_EQ(int_var, 88);
  std::remove(path.c_str());
}

static void test_set_parameter_file_int_vector() {
  reset();
  const string path = write_tmpfile("int_vec = {10, 20, 30}\n");
  set_parameter_file_globals(path);
  EXPECT_EQ(int_vec.size(), 3u);
  EXPECT_EQ(int_vec[0], 10);
  EXPECT_EQ(int_vec[1], 20);
  EXPECT_EQ(int_vec[2], 30);
  std::remove(path.c_str());
}

static void test_set_parameter_file_string_vector() {
  reset();
  const string path = write_tmpfile("string_vec = {foo, bar, baz}\n");
  set_parameter_file_globals(path);
  EXPECT_EQ(string_vec.size(), 3u);
  EXPECT_EQ(string_vec[0], string("foo"));
  EXPECT_EQ(string_vec[1], string("bar"));
  EXPECT_EQ(string_vec[2], string("baz"));
  std::remove(path.c_str());
}

static void test_set_parameter_file_missing() {
  EXPECT_EQ(set_parameter_file_globals("/tmp/globals_test_no_such_file.txt"), 0);
}

static void test_set_parameter_file_arg_with_file() {
  reset();
  const string path = write_tmpfile("int_var = 33\n");
  const char* argv[] = {"prog", path.c_str()};
  EXPECT_EQ(set_parameter_file_arg_globals(2, const_cast<char**>(argv)), 1);
  EXPECT_EQ(int_var, 33);
  std::remove(path.c_str());
}

static void test_set_parameter_file_arg_no_file_arg() {
  // argv[1] has '=' so it's not a filename; function returns 1 immediately.
  reset();
  const char* argv[] = {"prog", "int_var=1"};
  EXPECT_EQ(set_parameter_file_arg_globals(2, const_cast<char**>(argv)), 1);
  EXPECT_EQ(int_var, 42);  // unchanged
}

static void test_set_parameter_file_arg_no_args() {
  reset();
  const char* argv[] = {"prog"};
  EXPECT_EQ(set_parameter_file_arg_globals(1, const_cast<char**>(argv)), 1);
  EXPECT_EQ(int_var, 42);  // unchanged
}

static void test_command_line_overrides_param_file() {
  reset();
  const string path = write_tmpfile("int_var = 10\n");
  const char* argv[] = {"prog", path.c_str(), "int_var=20"};
  set_parameter_file_arg_globals(3, const_cast<char**>(argv));
  set_command_line_globals(3, const_cast<char**>(argv));
  EXPECT_EQ(int_var, 20);
  std::remove(path.c_str());
}

static void test_dump_globals_to_file() {
  reset();
  const string path = "/tmp/globals_test_dump.txt";
  dump_globals(path);
  std::ifstream f(path);
  string content((std::istreambuf_iterator<char>(f)),
                  std::istreambuf_iterator<char>());
  EXPECT_EQ(content.find("int_var") != string::npos, true);
  EXPECT_EQ(content.find("string_var") != string::npos, true);
  EXPECT_EQ(content.find("int_vec") != string::npos, true);
  std::remove(path.c_str());
}

// -----------------------------------------------------------------------------

int main() {
  test_defaults();
  test_set_command_line_int();
  test_set_command_line_double();
  test_set_command_line_string();
  test_set_command_line_int_vector();
  test_set_command_line_double_vector();
  test_set_command_line_string_vector();
  test_set_command_line_multiple();
  test_command_line_skips_first_arg_without_equals();
  test_set_command_line_unknown_var();
  test_set_parameter_file();
  test_set_parameter_file_comments();
  test_set_parameter_file_whitespace();
  test_set_parameter_file_int_vector();
  test_set_parameter_file_string_vector();
  test_set_parameter_file_missing();
  test_set_parameter_file_arg_with_file();
  test_set_parameter_file_arg_no_file_arg();
  test_set_parameter_file_arg_no_args();
  test_command_line_overrides_param_file();
  test_dump_globals_to_file();

  if (tests_failed > 0) {
    std::cerr << tests_failed << "/" << tests_run << " tests FAILED\n";
    return 1;
  }
  std::cout << tests_run << " tests passed\n";
  return 0;
}
