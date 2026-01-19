#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdio>
#include <vector>
#include <string>
#include "globals.h"

using std::string;
using std::vector;

int test_count = 0;
int passed_count = 0;

#define TEST(name) \
  void test_##name(); \
  void run_test_##name() { \
    std::cout << "Running test: " << #name << "..."; \
    test_##name(); \
    std::cout << " PASSED\n"; \
    passed_count++; \
  } \
  void test_##name()

#define EXPECT_EQ(a, b) \
  do { \
    test_count++; \
    if (!((a) == (b))) { \
      std::cerr << "\nAssertion failed: " << #a << " == " << #b << "\n"; \
      std::cerr << "  Expected: " << (b) << "\n"; \
      std::cerr << "  Actual: " << (a) << "\n"; \
      std::cerr << "  at " << __FILE__ << ":" << __LINE__ << "\n"; \
      exit(1); \
    } \
  } while (0)

#define EXPECT_DOUBLE_EQ(a, b) \
  do { \
    test_count++; \
    if (std::abs((a) - (b)) > 1e-6) { \
      std::cerr << "\nAssertion failed: " << #a << " == " << #b << "\n"; \
      std::cerr << "  Expected: " << (b) << "\n"; \
      std::cerr << "  Actual: " << (a) << "\n"; \
      std::cerr << "  at " << __FILE__ << ":" << __LINE__ << "\n"; \
      exit(1); \
    } \
  } while (0)

// Reset globals to initial values
void reset_globals() {
  f = 0;
  g = 10;
  x = 5;
  y = 10;
  s = "";
  vals = {1, 2, 3, 0, 0};
  doubles = {};
  names = {"abc", "def", "ghi"};
}

// Test basic scalar variable setting
TEST(SetScalarInt) {
  reset_globals();
  EXPECT_EQ(x, 5);
  set_global("x", "42");
  EXPECT_EQ(x, 42);
}

TEST(SetScalarDouble) {
  reset_globals();
  EXPECT_DOUBLE_EQ(f, 0.0);
  set_global("f", "3.14159");
  EXPECT_DOUBLE_EQ(f, 3.14159);
}

TEST(SetScalarString) {
  reset_globals();
  EXPECT_EQ(s, "");
  set_global("s", "hello world");
  EXPECT_EQ(s, "hello world");
}

TEST(SetNegativeInt) {
  reset_globals();
  set_global("x", "-100");
  EXPECT_EQ(x, -100);
}

TEST(SetNegativeDouble) {
  reset_globals();
  set_global("f", "-2.5");
  EXPECT_DOUBLE_EQ(f, -2.5);
}

TEST(SetZeroInt) {
  reset_globals();
  set_global("x", "0");
  EXPECT_EQ(x, 0);
}

TEST(SetZeroDouble) {
  reset_globals();
  set_global("f", "0.0");
  EXPECT_DOUBLE_EQ(f, 0.0);
}

TEST(InvalidIntConversion) {
  reset_globals();
  int old_value = x;
  int result = set_global("x", "not_a_number");
  EXPECT_EQ(result, 0);
  EXPECT_EQ(x, old_value);
}

TEST(InvalidDoubleConversion) {
  reset_globals();
  double old_value = f;
  int result = set_global("f", "abc123");
  EXPECT_EQ(result, 0);
  EXPECT_DOUBLE_EQ(f, old_value);
}

TEST(PartiallyValidNumber) {
  reset_globals();
  int old_value = x;
  int result = set_global("x", "123abc");
  EXPECT_EQ(result, 0);
  EXPECT_EQ(x, old_value);
}

TEST(IntegerOverflow) {
  reset_globals();
  int old_value = x;
  int result = set_global("x", "999999999999999999999");
  EXPECT_EQ(result, 0);
  EXPECT_EQ(x, old_value);
}

TEST(UnknownVariable) {
  reset_globals();
  int result = set_global("unknown_var", "123");
  EXPECT_EQ(result, 0);
}

TEST(SetIntVector) {
  reset_globals();
  set_global("vals", "{10, 20, 30}");
  EXPECT_EQ(vals.size(), 3);
  EXPECT_EQ(vals[0], 10);
  EXPECT_EQ(vals[1], 20);
  EXPECT_EQ(vals[2], 30);
}

TEST(SetDoubleVector) {
  reset_globals();
  set_global("doubles", "{1.1, 2.2, 3.3}");
  EXPECT_EQ(doubles.size(), 3);
  EXPECT_DOUBLE_EQ(doubles[0], 1.1);
  EXPECT_DOUBLE_EQ(doubles[1], 2.2);
  EXPECT_DOUBLE_EQ(doubles[2], 3.3);
}

TEST(SetStringVector) {
  reset_globals();
  set_global("names", "{alice, bob, charlie}");
  EXPECT_EQ(names.size(), 3);
  EXPECT_EQ(names[0], "alice");
  EXPECT_EQ(names[1], "bob");
  EXPECT_EQ(names[2], "charlie");
}

TEST(SetEmptyVector) {
  reset_globals();
  set_global("vals", "{}");
  EXPECT_EQ(vals.size(), 0);
}

TEST(SetSingleElementVector) {
  reset_globals();
  set_global("vals", "{99}");
  EXPECT_EQ(vals.size(), 1);
  EXPECT_EQ(vals[0], 99);
}

TEST(VectorWithWhitespace) {
  reset_globals();
  set_global("vals", "{  1  ,  2  ,  3  }");
  EXPECT_EQ(vals.size(), 3);
  EXPECT_EQ(vals[0], 1);
  EXPECT_EQ(vals[1], 2);
  EXPECT_EQ(vals[2], 3);
}

TEST(VectorWithNegativeNumbers) {
  reset_globals();
  set_global("vals", "{-1, -2, -3}");
  EXPECT_EQ(vals.size(), 3);
  EXPECT_EQ(vals[0], -1);
  EXPECT_EQ(vals[1], -2);
  EXPECT_EQ(vals[2], -3);
}

TEST(VectorWithoutBraces) {
  reset_globals();
  size_t old_size = vals.size();
  int result = set_global("vals", "1, 2, 3");
  EXPECT_EQ(result, 0);
  EXPECT_EQ(vals.size(), old_size);  // Should not change
}

TEST(VectorWithOnlyOpeningBrace) {
  reset_globals();
  size_t old_size = vals.size();
  int result = set_global("vals", "{1, 2, 3");
  EXPECT_EQ(result, 0);
  EXPECT_EQ(vals.size(), old_size);  // Should not change
}

TEST(VectorWithOnlyClosingBrace) {
  reset_globals();
  size_t old_size = vals.size();
  int result = set_global("vals", "1, 2, 3}");
  EXPECT_EQ(result, 0);
  EXPECT_EQ(vals.size(), old_size);  // Should not change
}

TEST(VectorWithInvalidElement) {
  reset_globals();
  size_t old_size = vals.size();
  int result = set_global("vals", "{1, abc, 3}");
  EXPECT_EQ(result, 0);
  EXPECT_EQ(vals.size(), old_size);  // Should not change
}

TEST(ParameterFileBasic) {
  reset_globals();
  std::ofstream ofs("test_params.txt");
  ofs << "x = 100\n";
  ofs << "f = 2.5\n";
  ofs << "s = test_string\n";
  ofs.close();

  int result = set_parameter_file_globals("test_params.txt");
  EXPECT_EQ(result, 1);
  EXPECT_EQ(x, 100);
  EXPECT_DOUBLE_EQ(f, 2.5);
  EXPECT_EQ(s, "test_string");

  std::remove("test_params.txt");
}

TEST(ParameterFileWithComments) {
  reset_globals();
  std::ofstream ofs("test_params.txt");
  ofs << "# This is a comment\n";
  ofs << "x = 200\n";
  ofs << "# Another comment\n";
  ofs << "f = 1.5\n";
  ofs.close();

  set_parameter_file_globals("test_params.txt");
  EXPECT_EQ(x, 200);
  EXPECT_DOUBLE_EQ(f, 1.5);

  std::remove("test_params.txt");
}

TEST(ParameterFileWithVectors) {
  reset_globals();
  std::ofstream ofs("test_params.txt");
  ofs << "vals = {100, 200, 300}\n";
  ofs << "names = {one, two, three}\n";
  ofs.close();

  set_parameter_file_globals("test_params.txt");
  EXPECT_EQ(vals.size(), 3);
  EXPECT_EQ(vals[0], 100);
  EXPECT_EQ(vals[1], 200);
  EXPECT_EQ(vals[2], 300);
  EXPECT_EQ(names.size(), 3);
  EXPECT_EQ(names[0], "one");
  EXPECT_EQ(names[1], "two");
  EXPECT_EQ(names[2], "three");

  std::remove("test_params.txt");
}

TEST(ParameterFileNonExistent) {
  reset_globals();
  int result = set_parameter_file_globals("nonexistent_file.txt");
  EXPECT_EQ(result, 0);
}

TEST(ScientificNotation) {
  reset_globals();
  set_global("f", "1.5e-10");
  EXPECT_DOUBLE_EQ(f, 1.5e-10);
}

TEST(ScientificNotationUpperCase) {
  reset_globals();
  set_global("f", "2.5E+5");
  EXPECT_DOUBLE_EQ(f, 2.5e+5);
}

int main() {
  std::cout << "Running simple tests...\n\n";

  // Run all tests
  run_test_SetScalarInt();
  run_test_SetScalarDouble();
  run_test_SetScalarString();
  run_test_SetNegativeInt();
  run_test_SetNegativeDouble();
  run_test_SetZeroInt();
  run_test_SetZeroDouble();
  run_test_InvalidIntConversion();
  run_test_InvalidDoubleConversion();
  run_test_PartiallyValidNumber();
  run_test_IntegerOverflow();
  run_test_UnknownVariable();
  run_test_SetIntVector();
  run_test_SetDoubleVector();
  run_test_SetStringVector();
  run_test_SetEmptyVector();
  run_test_SetSingleElementVector();
  run_test_VectorWithWhitespace();
  run_test_VectorWithNegativeNumbers();
  run_test_VectorWithoutBraces();
  run_test_VectorWithOnlyOpeningBrace();
  run_test_VectorWithOnlyClosingBrace();
  run_test_VectorWithInvalidElement();
  run_test_ParameterFileBasic();
  run_test_ParameterFileWithComments();
  run_test_ParameterFileWithVectors();
  run_test_ParameterFileNonExistent();
  run_test_ScientificNotation();
  run_test_ScientificNotationUpperCase();

  std::cout << "\n========================================\n";
  std::cout << "All tests passed! (" << passed_count << " tests, "
            << test_count << " assertions)\n";
  std::cout << "========================================\n";

  return 0;
}
