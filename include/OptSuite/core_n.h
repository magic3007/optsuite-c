/*
 * ==========================================================================
 *
 *       Filename:  core_n.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/31/2020 05:17:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */
#ifndef OPTSUITE_CORE_H
#define OPTSUITE_CORE_H

#include <memory>
#include <complex>
#include <numeric>
#include <limits>
#include "core_macro_n.h"

#ifdef OPTSUITE_USE_MKL
#ifndef EIGEN_USE_MKL_ALL
#define EIGEN_USE_MKL_ALL
#endif
#include <mkl.h>
#endif

#include <Eigen/Eigen>

namespace OptSuite {
    using Scalar = OPTSUITE_SCALAR_TYPE;
    using ComplexScalar = std::complex<Scalar>;
    using Index = OPTSUITE_DEFAULT_INDEX_TYPE;
    using SparseIndex = OPTSUITE_SPARSE_INDEX_TYPE;
    using Size = OPTSUITE_DEFAULT_INDEX_TYPE;
    using std::shared_ptr;

    using Eigen::Dynamic;
    using Eigen::Ref;
    using Eigen::ColMajor;
    using Eigen::RowMajor;
    using Eigen::EigenBase;
    using Eigen::Map;
    using Eigen::Stride;

    using Triplet = Eigen::Triplet<Scalar, Index>;
    using Mat = Eigen::Matrix<Scalar, Dynamic, Dynamic>;
    using Vec = Eigen::Matrix<Scalar, Dynamic, 1>;
    using SpMat = Eigen::SparseMatrix<Scalar, ColMajor, SparseIndex>;
    using SpVec = Eigen::SparseVector<Scalar>;
    using CMat = Eigen::Matrix<ComplexScalar, Dynamic, Dynamic>;
    using CVec = Eigen::Matrix<ComplexScalar, Dynamic, 1>;

    enum class Verbosity {
        // Suppress any output
        Quiet,
        // Output necessary information
        Info,
        // Output more detail during iteration
        Detail,
        // Output information for developer debugging
        Debug,
        // Output information that verbosely indicates the program state
        Verbose,
        // This option is only for testing program about logging.
        // Virtually the same as Verbose
        Everything
    };

    // convert to Scalar type
    constexpr Scalar operator"" _s(const long double x) {
        return static_cast<Scalar>(x);
    }

    // convert to Scalar type
    constexpr Scalar operator"" _s(const unsigned long long int x) {
        return static_cast<Scalar>(x);
    }

    constexpr Index operator"" _i(const unsigned long long int x) {
        return static_cast<Index>(x);
    }

    // imaginary part for complex number
    constexpr std::complex<Scalar> operator"" _ii(const long double x){
        return std::complex<Scalar>{0.0_s, static_cast<Scalar>(x)};
    }
    constexpr std::complex<Scalar> operator"" _ii(const unsigned long long x){
        return std::complex<Scalar>{0.0_s, static_cast<Scalar>(x)};
    }
    

    // define machine accuracy
    constexpr Scalar eps = std::numeric_limits<Scalar>::epsilon();


    template <typename Derived>
    constexpr bool is_eigen_type_f(const EigenBase<Derived>*) {
        return true;
    }
    constexpr bool is_eigen_type_f(...) {
        return false;
    }

    // is_eigen_type<T>: true if T is derived from EigenBase<T>, false otherwise
    template <typename T>
    struct is_eigen_type {
        static constexpr bool value = 
            is_eigen_type_f(static_cast<typename std::add_pointer<T>::type>(nullptr));
    };

    // is_optsuite_scalar_type<T>: true if T == Scalar or ComplexScalar
    template <typename T>
    struct is_optsuite_scalar_type {
        static constexpr bool value = 
            std::is_same<T, Scalar>::value || std::is_same<T, ComplexScalar>::value;
    };

    // is_scalar_type<T>: true if T is integer or floating-point number
    template <typename T>
    struct is_scalar_type {
        static constexpr bool value = std::is_arithmetic<T>::value;
    };

    // is_value_type<T>: true if T is either eigen type or scalar type
    template <typename T>
    struct is_value_type {
        static constexpr bool value = is_eigen_type<T>::value || is_scalar_type<T>::value;
    };
}

#endif
