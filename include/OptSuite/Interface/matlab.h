/*
 * ==========================================================================
 *
 *       Filename:  matlab.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/28/2020 12:28:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#ifndef OPTSUITE_INTERFACE_MATLAB_H
#define OPTSUITE_INTERFACE_MATLAB_H

#include <cstring>
#include <type_traits>
#include "OptSuite/core_n.h"
#include "OptSuite/Utils/logger.h"
#include "matrix.h"
#include "mat.h"

namespace OptSuite { namespace Interface {
#if OPTSUITE_SCALAR_TOKEN == 0
    inline bool mx_is_scalar(const mxArray *p){
        return mxIsDouble(p) && !mxIsComplex(p);
    }
    inline bool mx_is_complex_scalar(const mxArray *p){
        return mxIsDouble(p) && mxIsComplex(p);
    }
#elif OPTSUITE_SCALAR_TOKEN == 1
    inline bool mx_is_scalar(const mxArray *p){
        return mxIsSingle(p) && !mxIsComplex(p);
    }
    inline bool mx_is_complex_scalar(const mxArray *p){
        return mxIsSingle(p) && mxIsComplex(p);
    }
#endif
    template<typename T>
    void get_matrix_from_file(const char *, const char *,
                              Eigen::Matrix<T, Dynamic, Dynamic> &);
    template<typename T>
    void get_matrix_from_file(const char *, const char *,
                              Eigen::SparseMatrix<T, ColMajor, SparseIndex> &);
    int read_variable_from_mat(const char *, const char *,
                              MATFile **, mxArray **);
    template<typename T>
    int mxArray_to_dense(const mxArray *, Eigen::Matrix<T, Dynamic, Dynamic> &);

    template<typename T>
    int mxArray_to_sparse(const mxArray *, Eigen::SparseMatrix<T, ColMajor, SparseIndex> &);
}}

#endif
