/*
 * ==========================================================================
 *
 *       Filename:  matlab.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/28/2020 02:43:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#include "OptSuite/core_n.h"
#include "OptSuite/Utils/logger.h"
#include "OptSuite/Interface/matlab.h"

namespace OptSuite { namespace Interface {
    template<typename T>
    void get_matrix_from_file(const char *filename, const char *varname,
                              Eigen::Matrix<T, Dynamic, Dynamic> & mat){
        MATFile *pmat;
        mxArray *pa;
        int info;

        // read from .mat
        info = read_variable_from_mat(filename, varname, &pmat, &pa);
        if (info) return;

        // convert to Eigen dense
        info = mxArray_to_dense(pa, mat);

        if (info){
            Utils::Global::logger_e.log_info(
                    "Failed to read variable \'", varname, "\': type mismatch.\n");
        }

        mxDestroyArray(pa);
        matClose(pmat);
    }

    template<typename T>
    void get_matrix_from_file(const char *filename, const char *varname,
                              Eigen::SparseMatrix<T, ColMajor, SparseIndex> & spmat){
        MATFile *pmat;
        mxArray *pa;
        int info;

        // read from .mat
        info = read_variable_from_mat(filename, varname, &pmat, &pa);
        if (info) return;

        // convert to Eigen sparse
        info = mxArray_to_sparse(pa, spmat);

        if (info){
            Utils::Global::logger_e.log_info(
                    "Failed to read variable \'", varname, "\': type mismatch.\n");
        }

        mxDestroyArray(pa);
        matClose(pmat);
    }


    int read_variable_from_mat(const char *filename, const char *varname,
                               MATFile **pmat, mxArray **pa){
        using namespace Utils::Global;
        // open mat file
        *pmat = matOpen(filename, "r");
        if (*pmat == NULL){
            logger_e.log_info("Failed to open file \'", filename, "\'.\n");
            return 1;
        }

        // extract specified variable
        *pa = matGetVariable(*pmat, varname);
        if (*pa == NULL){
            logger_e.log_info("No variable named \'", varname, "\' found.\n");
            matClose(*pmat);
            return 1;
        }

        return 0;
    }

    template<typename T>
    int mxArray_to_dense(const mxArray *pa,
                          Eigen::Matrix<T, Dynamic, Dynamic> & mat){
        // check type
        bool matched = (std::is_same<T, Scalar>::value && mx_is_scalar(pa)) ||
            (std::is_same<T, ComplexScalar>::value && mx_is_complex_scalar(pa));
        matched = matched && !mxIsSparse(pa);

        if (matched && !mxIsEmpty(pa)){
            mwSize m = mxGetM(pa);
            mwSize n = mxGetN(pa);
            T* data_ptr = (T*)mxGetPr(pa); 

            // resize and copy
            mat.resize(m, n);
            memcpy(mat.data(), data_ptr, m * n * sizeof(T));

        } else {
            return 1;
        }
        return 0;

    }

    template<typename T>
    int mxArray_to_sparse(const mxArray *pa,
                          Eigen::SparseMatrix<T, ColMajor, SparseIndex> & spmat){
        // check type
        bool matched = (std::is_same<T, Scalar>::value && mx_is_scalar(pa)) ||
            (std::is_same<T, ComplexScalar>::value && mx_is_complex_scalar(pa));
        matched = matched && mxIsSparse(pa);

        if (matched && !mxIsEmpty(pa)){
            mwSize m = mxGetM(pa);
            mwSize n = mxGetN(pa);
            mwSize nnz = mxGetNzmax(pa);
            T* data_ptr = (T*)mxGetPr(pa);

            mwIndex *jc = mxGetJc(pa);
            mwIndex *ir = mxGetIr(pa);

            // MATLAB uses unsigned indexing, Eigen uses signed indexing
            // Thus it is safer to use elementwise conversion
            spmat.resize(m, n);
            spmat.reserve(nnz);

            SparseIndex *sp_jc = spmat.outerIndexPtr();
            SparseIndex *sp_ir = spmat.innerIndexPtr();

            for (mwSize i = 0; i < n + 1; ++i){
                *sp_jc++ = *jc++;
            }

            for (mwSize i = 0; i< nnz; ++i){
                *sp_ir++ = *ir++;
            }

            std::memcpy(spmat.valuePtr(), data_ptr, nnz * sizeof(Scalar));

        } else {
            return 1;
        }
        return 0;
    }

    // instantiate
    template void get_matrix_from_file(const char *, const char *,
            Eigen::Matrix<Scalar, Dynamic, Dynamic>&);
    template void get_matrix_from_file(const char *, const char *,
            Eigen::Matrix<ComplexScalar, Dynamic, Dynamic>&);
    template void get_matrix_from_file(const char *, const char *,
            Eigen::SparseMatrix<Scalar, ColMajor, SparseIndex>&);
    template void get_matrix_from_file(const char *, const char *,
            Eigen::SparseMatrix<ComplexScalar, ColMajor, SparseIndex>&);

}}
