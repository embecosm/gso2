#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([GNU Superoptimizer Toolkit], [0.1], [james.pallister@embecosm.com])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
# AC_CONFIG_SRCDIR([src/])
AM_INIT_AUTOMAKE([subdir-objects])
AM_MAINTAINER_MODE([disable])

: ${CXXFLAGS="-g"}

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET

AX_CXX_COMPILE_STDCXX_11

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lboost_unit_test_framework':
AC_CHECK_LIB([boost_unit_test_framework], [main])

# Checks for header files.
AC_CHECK_HEADERS([stdint.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([strerror])

# Link in test files
AC_CONFIG_LINKS([
    tests/data/long_1.csv:tests/data/long_1.csv
    tests/data/long_2.csv:tests/data/long_2.csv
    tests/data/sparse_4.csv:tests/data/sparse_4.csv
    tests/data/sparse_5.csv:tests/data/sparse_5.csv
    tests/data/standard_1.csv:tests/data/standard_1.csv
    tests/data/standard_2.csv:tests/data/standard_2.csv
    ])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
