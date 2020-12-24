#ifndef OPTSUITE_CORE_MACRO_H
#define OPTSUITE_CORE_MACRO_H

#include "OptSuite/config.h"

#ifndef OPTSUITE_DEFAULT_INDEX_TYPE
#define OPTSUITE_DEFAULT_INDEX_TYPE ::std::ptrdiff_t
#endif

// Type token for scalar value. 0: double  1: float
#ifndef OPTSUITE_SCALAR_TOKEN
#define OPTSUITE_SCALAR_TOKEN 0
#endif

// Type for scalar value. Typically double.
// Only float and double is supported for MKL and SuiteSparse.
#if OPTSUITE_SCALAR_TOKEN == 0
#define OPTSUITE_SCALAR_TYPE double
#elif OPTSUITE_SCALAR_TOKEN == 1
#define OPTSUITE_SCALAR_TYPE float
#else
#error "unsupported scalar type"
#endif

// Index type for sparse matrix. Typically int.
// Only int is supported for SuiteSparse.
#ifndef OPTSUITE_SPARSE_INDEX_TYPE
#define OPTSUITE_SPARSE_INDEX_TYPE int
#endif

// define to use MKL
// This macro should probably defined by project or cmake,
// instead of defining here.
// #define OPTSUITE_USE_MKL

// define to use SuiteSparse
// This macro should probably defined by project or cmake,
// instead of defining here.
// #define OPTSUITE_USE_SUITE_SPARSE

// define to disable auto linking for SuiteSparse (msvc only)
// Auto linking should be handled by SuiteSparse... But it isn't.
// #define OPTSUITE_DISABLE_SUITE_SPARSE_AUTO_LINK

// define to disable multi threading at compile time
#ifdef OPTSUITE_DONT_PARALLELIZE
#define EIGEN_DONT_PARALLELIZE
#endif

// define to enable the use of std::printf, std::puts
// and anything concerning stdin, stdout, stderr
// if defined to 0, these functions will be implemented
// with C++ style IO
#ifndef OPTSUITE_USE_C_STYLE_IO
#define OPTSUITE_USE_C_STYLE_IO 1
#endif

// define to use Eigen's built-in eigenvalue solver
// may use MKL's solver if not defined
// You may probably want to define this when using scalar
// type other than float and double.
// #define OPTSUITE_FORCE_EIGEN_EIGEN_SOLVER
#if defined(OPTSUITE_USE_MKL) && !defined(OPTSUITE_FORCE_EIGEN_EIGEN_SOLVER)
#define OPTSUITE_MKL_EIG_ENABLED
#endif

// Set to 1 to enable the use of MKL to process sparse matrix
// Poor performance in parallel mode. Not recommended.
#define OPTSUITE_USE_MKL_SPARSE 0

#if defined(OPTSUITE_USE_MKL) && OPTSUITE_USE_MKL_SPARSE
#define OPTSUITE_MKL_SPARSE_ENABLED
#endif

#ifndef EIGEN_DEFAULT_DENSE_INDEX_TYPE
#define EIGEN_DEFAULT_DENSE_INDEX_TYPE OPTSUITE_DEFAULT_INDEX_TYPE
#endif

// define to stop defining NDEBUG when release mode is detected
// #define OPTSUITE_NO_DEFINE_NDEBUG

// Compiler detection copied from Eigen
#ifdef __GNUC__
  #define OPTSUITE_GNUC 1
#else
  #define OPTSUITE_GNUC 0
#endif

#if defined(__clang__)
  #define OPTSUITE_CLANG (__clang_major__*100+__clang_minor__)
#else
  #define OPTSUITE_CLANG 0
#endif

#if defined(__llvm__)
  #define OPTSUITE_LLVM 1
#else
  #define OPTSUITE_LLVM 0
#endif

#if defined(__INTEL_COMPILER)
  #define OPTSUITE_ICC __INTEL_COMPILER
#else
  #define OPTSUITE_ICC 0
#endif

#if defined(__MINGW32__)
  #define OPTSUITE_MINGW 1
#else
  #define OPTSUITE_MINGW 0
#endif

#if defined(__SUNPRO_CC)
  #define OPTSUITE_SUNCC 1
#else
  #define OPTSUITE_SUNCC 0
#endif

#if defined(_MSC_VER)
  #define OPTSUITE_MSVC _MSC_VER
#else
  #define OPTSUITE_MSVC 0
#endif

// Visual Studio 2017 ==> 1910
#if OPTSUITE_MSVC && OPTSUITE_MSVC < 1910
#error Visual Studio with a version older than 2017 is not supported
#endif

// MSVC does not automatically define NDEBUG in Release configuration
#if OPTSUITE_MSVC && !defined(_DEBUG) && !defined(NDEBUG) && !defined(OPTSUITE_NO_DEFINE_NDEBUG)
#define NDEBUG
#endif

// define to debug ssnsdp behavior when developing
#if !defined(NDEBUG) && !defined(OPTSUITE_INTERNAL_DEBUG)
#define OPTSUITE_INTERNAL_DEBUG
#endif

// Global verbosity at compile time
// To change verbosity at runtime, use the verbosity of a model
#ifndef OPTSUITE_VERBOSITY
#ifndef NDEBUG
#define OPTSUITE_VERBOSITY ::ssnsdp::Verbosity::Debug
#else
#define OPTSUITE_VERBOSITY ::ssnsdp::Verbosity::Info
#endif
#endif

// Assertion. Used when wrong input may violate the condition.
// By default, it is simply a wrapper to assert.
// May be overrided to throw an exception, etc.
#ifndef OPTSUITE_ASSERT
#define OPTSUITE_ASSERT(expr) assert(expr)
#endif

#ifndef OPTSUITE_ASSERT_MSG
#define OPTSUITE_ASSERT_MSG(expr, msg) assert((expr) && (msg))
#endif

// void expression that marks unreachable code
#if OPTSUITE_GNUC || OPTSUITE_CLANG
#define OPTSUITE_INTERNAL_UNREACHABLE() __builtin_unreachable()
#endif

// Assumption. Used when there is no way to violate the assumption, whatever the input.
// By default, violation of assumptions terminates the program in debug mode,
// and lead to undefined behavior in release mode.
#ifndef OPTSUITE_ASSUME
#ifndef NDEBUG
#define OPTSUITE_ASSUME(expr) assert(expr)
#else
#if OPTSUITE_MSVC
#define OPTSUITE_ASSUME(expr) __assume(expr)
#elif defined(OPTSUITE_INTERNAL_UNREACHABLE)
#define OPTSUITE_ASSUME(expr) ( (expr) ? (void)0 : OPTSUITE_INTERNAL_UNREACHABLE() )
#else
#define OPTSUITE_ASSUME(expr) ((void)0)
#endif // COMPILER TEST
#endif // #ifndef NDEBUG
#endif // #ifndef OPTSUITE_ASSUME

#ifndef OPTSUITE_UNREACHABLE
#if defined(OPTSUITE_INTERNAL_UNREACHABLE) && defined(NDEBUG)
#define OPTSUITE_UNREACHABLE() OPTSUITE_INTERNAL_UNREACHABLE()
#else
#define OPTSUITE_UNREACHABLE() OPTSUITE_ASSUME(false)
#endif
#endif

// encourage compiler to inline a function
#ifndef OPTSUITE_STRONG_INLINE
#if OPTSUITE_MSVC || OPTSUITE_ICC
#define OPTSUITE_STRONG_INLINE __forceinline
#else
#define OPTSUITE_STRONG_INLINE inline
#endif
#endif

// add an "inline" keyword to specify that multiple definitions are permitted
// Use this macro to state explicitly that inlining is not preferred
#ifndef OPTSUITE_WEAK_INLINE
#define OPTSUITE_WEAK_INLINE inline
#endif

// Mark a converting constructor as non-explicit
#ifndef OPTSUITE_IMPLICIT
#define OPTSUITE_IMPLICIT
#endif

// Suppress [[maybe_unused]] has no effect warning of g++
#if OPTSUITE_GNUC
#define OPTSUITE_CLASS_MEMBER_MAYBE_UNUSED
#else
#define OPTSUITE_CLASS_MEMBER_MAYBE_UNUSED [[maybe_unused]]
#endif

// explicitly exporting shared library functions
#if OPTSUITE_MSVC
#define OPTSUITE_EXPORT __declspec(dllexport)
#else
#define OPTSUITE_EXPORT __attribute__ ((visibility("default")))
#endif

#endif
