// Test global variable definitions used by globals_test.cc.
GLOBAL(int, int_var, 42)
GLOBAL(double, double_var, 1.5)
GLOBAL(string, string_var, "hello")
GLOBALV(int, int_vec, ({1, 2, 3}))
GLOBALV(double, double_vec, ({0.5, 1.5}))
GLOBALV(string, string_vec, ({"a", "b", "c"}))
