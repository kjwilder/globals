#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <cstdio>
#include "globals.h"

// Test fixture for globals tests
class GlobalsTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Reset global variables to their initial values
    f = 0;
    g = 10;
    x = 5;
    y = 10;
    s = "";
    vals = {1, 2, 3, 0, 0};
    doubles = {};
    names = {"abc", "def", "ghi"};
  }

  void TearDown() override {
    // Clean up any temporary files
    std::remove("test_params.txt");
    std::remove("test_dump.txt");
  }

  // Helper function to create a temporary parameter file
  void CreateParamFile(const std::string& filename, const std::string& content) {
    std::ofstream ofs(filename);
    ofs << content;
    ofs.close();
  }
};

// Test basic scalar variable setting
TEST_F(GlobalsTest, SetScalarInt) {
  EXPECT_EQ(x, 5);  // Initial value
  set_global("x", "42");
  EXPECT_EQ(x, 42);
}

TEST_F(GlobalsTest, SetScalarDouble) {
  EXPECT_EQ(f, 0);
  set_global("f", "3.14159");
  EXPECT_DOUBLE_EQ(f, 3.14159);
}

TEST_F(GlobalsTest, SetScalarString) {
  EXPECT_EQ(s, "");
  set_global("s", "hello world");
  EXPECT_EQ(s, "hello world");
}

// Test negative numbers
TEST_F(GlobalsTest, SetNegativeInt) {
  set_global("x", "-100");
  EXPECT_EQ(x, -100);
}

TEST_F(GlobalsTest, SetNegativeDouble) {
  set_global("f", "-2.5");
  EXPECT_DOUBLE_EQ(f, -2.5);
}

// Test zero values
TEST_F(GlobalsTest, SetZeroInt) {
  set_global("x", "0");
  EXPECT_EQ(x, 0);
}

TEST_F(GlobalsTest, SetZeroDouble) {
  set_global("f", "0.0");
  EXPECT_DOUBLE_EQ(f, 0.0);
}

// Test invalid conversions
TEST_F(GlobalsTest, InvalidIntConversion) {
  int old_value = x;
  int result = set_global("x", "not_a_number");
  EXPECT_EQ(result, 0);  // Should fail
  EXPECT_EQ(x, old_value);  // Value should not change
}

TEST_F(GlobalsTest, InvalidDoubleConversion) {
  double old_value = f;
  int result = set_global("f", "abc123");
  EXPECT_EQ(result, 0);  // Should fail
  EXPECT_DOUBLE_EQ(f, old_value);  // Value should not change
}

TEST_F(GlobalsTest, PartiallyValidNumber) {
  int old_value = x;
  int result = set_global("x", "123abc");
  EXPECT_EQ(result, 0);  // Should fail (has trailing characters)
  EXPECT_EQ(x, old_value);
}

// Test integer overflow
TEST_F(GlobalsTest, IntegerOverflow) {
  int old_value = x;
  int result = set_global("x", "999999999999999999999");
  EXPECT_EQ(result, 0);  // Should fail on overflow
  EXPECT_EQ(x, old_value);
}

// Test unknown variable
TEST_F(GlobalsTest, UnknownVariable) {
  int result = set_global("unknown_var", "123");
  EXPECT_EQ(result, 0);  // Should fail
}

// Test vector setting
TEST_F(GlobalsTest, SetIntVector) {
  set_global("vals", "{10, 20, 30}");
  ASSERT_EQ(vals.size(), 3);
  EXPECT_EQ(vals[0], 10);
  EXPECT_EQ(vals[1], 20);
  EXPECT_EQ(vals[2], 30);
}

TEST_F(GlobalsTest, SetDoubleVector) {
  set_global("doubles", "{1.1, 2.2, 3.3}");
  ASSERT_EQ(doubles.size(), 3);
  EXPECT_DOUBLE_EQ(doubles[0], 1.1);
  EXPECT_DOUBLE_EQ(doubles[1], 2.2);
  EXPECT_DOUBLE_EQ(doubles[2], 3.3);
}

TEST_F(GlobalsTest, SetStringVector) {
  set_global("names", "{alice, bob, charlie}");
  ASSERT_EQ(names.size(), 3);
  EXPECT_EQ(names[0], "alice");
  EXPECT_EQ(names[1], "bob");
  EXPECT_EQ(names[2], "charlie");
}

TEST_F(GlobalsTest, SetEmptyVector) {
  set_global("vals", "{}");
  EXPECT_EQ(vals.size(), 0);
}

TEST_F(GlobalsTest, SetSingleElementVector) {
  set_global("vals", "{99}");
  ASSERT_EQ(vals.size(), 1);
  EXPECT_EQ(vals[0], 99);
}

// Test vector with whitespace
TEST_F(GlobalsTest, VectorWithWhitespace) {
  set_global("vals", "{  1  ,  2  ,  3  }");
  ASSERT_EQ(vals.size(), 3);
  EXPECT_EQ(vals[0], 1);
  EXPECT_EQ(vals[1], 2);
  EXPECT_EQ(vals[2], 3);
}

// Test vector with negative numbers
TEST_F(GlobalsTest, VectorWithNegativeNumbers) {
  set_global("vals", "{-1, -2, -3}");
  ASSERT_EQ(vals.size(), 3);
  EXPECT_EQ(vals[0], -1);
  EXPECT_EQ(vals[1], -2);
  EXPECT_EQ(vals[2], -3);
}

// Test vector without braces (should fail)
TEST_F(GlobalsTest, VectorWithoutBraces) {
  std::vector<int> old_vals = vals;
  int result = set_global("vals", "1, 2, 3");
  EXPECT_EQ(result, 0);  // Should fail
  EXPECT_EQ(vals, old_vals);  // Value should not change
}

// Test vector with only opening brace (should fail)
TEST_F(GlobalsTest, VectorWithOnlyOpeningBrace) {
  std::vector<int> old_vals = vals;
  int result = set_global("vals", "{1, 2, 3");
  EXPECT_EQ(result, 0);  // Should fail
  EXPECT_EQ(vals, old_vals);
}

// Test vector with only closing brace (should fail)
TEST_F(GlobalsTest, VectorWithOnlyClosingBrace) {
  std::vector<int> old_vals = vals;
  int result = set_global("vals", "1, 2, 3}");
  EXPECT_EQ(result, 0);  // Should fail
  EXPECT_EQ(vals, old_vals);
}

// Test vector with invalid element
TEST_F(GlobalsTest, VectorWithInvalidElement) {
  std::vector<int> old_vals = vals;
  int result = set_global("vals", "{1, abc, 3}");
  EXPECT_EQ(result, 0);  // Should fail
  EXPECT_EQ(vals, old_vals);
}

// Test parameter file parsing
TEST_F(GlobalsTest, ParameterFileBasic) {
  CreateParamFile("test_params.txt",
                  "x = 100\n"
                  "f = 2.5\n"
                  "s = test_string\n");

  int result = set_parameter_file_globals("test_params.txt");
  EXPECT_EQ(result, 1);
  EXPECT_EQ(x, 100);
  EXPECT_DOUBLE_EQ(f, 2.5);
  EXPECT_EQ(s, "test_string");
}

TEST_F(GlobalsTest, ParameterFileWithComments) {
  CreateParamFile("test_params.txt",
                  "# This is a comment\n"
                  "x = 200\n"
                  "# Another comment\n"
                  "f = 1.5\n");

  set_parameter_file_globals("test_params.txt");
  EXPECT_EQ(x, 200);
  EXPECT_DOUBLE_EQ(f, 1.5);
}

TEST_F(GlobalsTest, ParameterFileWithVectors) {
  CreateParamFile("test_params.txt",
                  "vals = {100, 200, 300}\n"
                  "names = {one, two, three}\n");

  set_parameter_file_globals("test_params.txt");
  ASSERT_EQ(vals.size(), 3);
  EXPECT_EQ(vals[0], 100);
  EXPECT_EQ(vals[1], 200);
  EXPECT_EQ(vals[2], 300);
  ASSERT_EQ(names.size(), 3);
  EXPECT_EQ(names[0], "one");
  EXPECT_EQ(names[1], "two");
  EXPECT_EQ(names[2], "three");
}

TEST_F(GlobalsTest, ParameterFileWithWhitespace) {
  CreateParamFile("test_params.txt",
                  "  x   =   150  \n"
                  "f=3.0\n"
                  "s=    no_spaces_in_value   \n");

  set_parameter_file_globals("test_params.txt");
  EXPECT_EQ(x, 150);
  EXPECT_DOUBLE_EQ(f, 3.0);
  EXPECT_EQ(s, "no_spaces_in_value");
}

TEST_F(GlobalsTest, ParameterFileNonExistent) {
  int result = set_parameter_file_globals("nonexistent_file.txt");
  EXPECT_EQ(result, 0);  // Should fail
}

TEST_F(GlobalsTest, ParameterFileWithUnknownVariable) {
  CreateParamFile("test_params.txt",
                  "x = 100\n"
                  "unknown_var = 200\n"
                  "f = 2.5\n");

  // Should still process valid variables despite unknown ones
  int result = set_parameter_file_globals("test_params.txt");
  EXPECT_EQ(result, 1);  // Still succeeds
  EXPECT_EQ(x, 100);
  EXPECT_DOUBLE_EQ(f, 2.5);
}

// Test command line parsing
TEST_F(GlobalsTest, CommandLineBasic) {
  char* argv[] = {
    (char*)"program",
    (char*)"x=99",
    (char*)"f=1.23",
    (char*)"s=cmdline"
  };
  int argc = 4;

  set_command_line_globals(argc, argv);
  EXPECT_EQ(x, 99);
  EXPECT_DOUBLE_EQ(f, 1.23);
  EXPECT_EQ(s, "cmdline");
}

TEST_F(GlobalsTest, CommandLineWithVectors) {
  char* argv[] = {
    (char*)"program",
    (char*)"vals={5,10,15}",
    (char*)"names={foo,bar}"
  };
  int argc = 3;

  set_command_line_globals(argc, argv);
  ASSERT_EQ(vals.size(), 3);
  EXPECT_EQ(vals[0], 5);
  EXPECT_EQ(vals[1], 10);
  EXPECT_EQ(vals[2], 15);
  ASSERT_EQ(names.size(), 2);
  EXPECT_EQ(names[0], "foo");
  EXPECT_EQ(names[1], "bar");
}

TEST_F(GlobalsTest, CommandLineSkipsParamFile) {
  CreateParamFile("test_params.txt", "x = 50\n");

  char* argv[] = {
    (char*)"program",
    (char*)"test_params.txt",  // Should be skipped
    (char*)"y=77"
  };
  int argc = 3;

  set_command_line_globals(argc, argv);
  EXPECT_EQ(y, 77);
  EXPECT_NE(x, 50);  // Param file should not be processed
}

// Test parameter file from argv
TEST_F(GlobalsTest, ParameterFileArg) {
  CreateParamFile("test_params.txt", "x = 888\n");

  char* argv[] = {
    (char*)"program",
    (char*)"test_params.txt"
  };
  int argc = 2;

  int result = set_parameter_file_arg_globals(argc, argv);
  EXPECT_EQ(result, 1);
  EXPECT_EQ(x, 888);
}

TEST_F(GlobalsTest, ParameterFileArgWithCommandLine) {
  CreateParamFile("test_params.txt",
                  "x = 100\n"
                  "f = 1.0\n");

  char* argv[] = {
    (char*)"program",
    (char*)"test_params.txt",
    (char*)"x=200",  // Should override param file
    (char*)"y=50"
  };
  int argc = 4;

  set_parameter_file_arg_globals(argc, argv);
  set_command_line_globals(argc, argv);

  EXPECT_EQ(x, 200);  // Overridden by command line
  EXPECT_DOUBLE_EQ(f, 1.0);  // From param file
  EXPECT_EQ(y, 50);  // From command line
}

// Test dump_globals
TEST_F(GlobalsTest, DumpGlobalsToFile) {
  x = 42;
  f = 3.14;
  s = "test";
  vals = {1, 2, 3};

  dump_globals("test_dump.txt");

  std::ifstream ifs("test_dump.txt");
  ASSERT_TRUE(ifs.is_open());

  std::stringstream buffer;
  buffer << ifs.rdbuf();
  std::string content = buffer.str();

  // Check that the file contains our variables
  EXPECT_NE(content.find("x = 42"), std::string::npos);
  EXPECT_NE(content.find("f = 3.14"), std::string::npos);
  EXPECT_NE(content.find("s = \"test\""), std::string::npos);
  EXPECT_NE(content.find("vals = {1, 2, 3}"), std::string::npos);
}

// Test edge cases
TEST_F(GlobalsTest, EmptyString) {
  set_global("s", "");
  EXPECT_EQ(s, "");
}

TEST_F(GlobalsTest, StringWithSpaces) {
  set_global("s", "hello world with spaces");
  EXPECT_EQ(s, "hello world with spaces");
}

TEST_F(GlobalsTest, VeryLargeVector) {
  std::string vec_str = "{";
  for (int i = 0; i < 1000; ++i) {
    if (i > 0) vec_str += ",";
    vec_str += std::to_string(i);
  }
  vec_str += "}";

  set_global("vals", vec_str);
  ASSERT_EQ(vals.size(), 1000);
  EXPECT_EQ(vals[0], 0);
  EXPECT_EQ(vals[999], 999);
}

// Test scientific notation for doubles
TEST_F(GlobalsTest, ScientificNotation) {
  set_global("f", "1.5e-10");
  EXPECT_DOUBLE_EQ(f, 1.5e-10);
}

TEST_F(GlobalsTest, ScientificNotationUpperCase) {
  set_global("f", "2.5E+5");
  EXPECT_DOUBLE_EQ(f, 2.5e+5);
}

// Test that variables retain values between calls
TEST_F(GlobalsTest, ValuePersistence) {
  set_global("x", "10");
  EXPECT_EQ(x, 10);

  set_global("y", "20");
  EXPECT_EQ(y, 20);
  EXPECT_EQ(x, 10);  // x should still be 10
}

// Main function to run all tests
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
