/*
 * ==========================================================================
 *
 *       Filename:  spmat_wrapper.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/10/2020 11:14:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#ifndef OPTSUITE_BASE_SPMAT_WRAPPER_H
#define OPTSUITE_BASE_SPMAT_WRAPPER_H

#include "OptSuite/core_n.h"
#include "OptSuite/Base/variable.h"
#include "OptSuite/Base/mat_wrapper.h"

namespace OptSuite { namespace Base {
    template<typename T> class MatWrapper;

    template<typename dtype>
    class SpMatWrapper : public Variable<dtype> {
        using spmat_t = Eigen::SparseMatrix<dtype, ColMajor, SparseIndex>;
        spmat_t data_;
        
        public:
            SpMatWrapper() = default;
            SpMatWrapper(const Ref<const spmat_t> A) : data_(A) {}
            ~SpMatWrapper() = default;

            inline spmat_t& spmat() { return data_; }
            inline const spmat_t& spmat() const { return data_; }

            Variable<dtype>& operator=(const Variable<dtype>& other){
                const SpMatWrapper<dtype>* other_ptr = dynamic_cast<const SpMatWrapper<dtype>*>(&other);
                OPTSUITE_ASSERT(other_ptr);
                data_ = other_ptr->data_;
                return *this;
            }

            inline dtype dot(const SpMatWrapper& other) const {
                return data_.conjugate().cwiseProduct(other.data_).sum();
            }

            inline dtype dot(const MatWrapper<dtype>& other) const {
                return data_.conjugate().cwiseProduct(other.mat()).sum();
            }

            inline dtype dot(const Variable<dtype>& other) const {
                const MatWrapper<dtype>* other_ptr = dynamic_cast<const MatWrapper<dtype>*>(&other);
                const SpMatWrapper* other_ptr_s = dynamic_cast<const SpMatWrapper*>(&other);
                OPTSUITE_ASSERT(other_ptr || other_ptr_s);
                if (other_ptr)
                    return this->dot(*other_ptr);
                else
                    return this->dot(*other_ptr_s);

            }

            inline void set_zero_like(const Ref<const spmat_t>& other){
                data_ = other;
                data_.setZero();
            }

            inline void set_zero_like(const Variable<dtype>& other){
                const SpMatWrapper* other_ptr = dynamic_cast<const SpMatWrapper*>(&other);
                OPTSUITE_ASSERT(other_ptr);
                data_ = other_ptr->data_;
                data_.setZero();
            }

            inline void set_zero_like(Index rows, Index cols, Index nnz,
                    const SparseIndex* outer, const SparseIndex* inner){
                Scalar *buff = new Scalar[nnz];
                data_ = Map<const spmat_t>(rows, cols, nnz, outer, inner, buff);
                delete []buff;
            }

    };

}}

#endif
