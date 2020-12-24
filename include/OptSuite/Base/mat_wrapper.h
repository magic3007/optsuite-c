/*
 * ===========================================================================
 *
 *       Filename:  mat_wrapper.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/11/2020 01:26:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, Peking University
 *
 * ===========================================================================
 */

#ifndef OPTSUITE_BASE_MAT_WRAPPER_H
#define OPTSUITE_BASE_MAT_WRAPPER_H

#include "OptSuite/core_n.h"
#include "OptSuite/Base/variable.h"
#include "OptSuite/Base/spmat_wrapper.h"

namespace OptSuite { namespace Base {
    template<typename T> class SpMatWrapper;

    template<typename dtype>
    class MatWrapper : public Variable<dtype> {
        using mat_t = Eigen::Matrix<dtype, Dynamic, Dynamic>;
        mat_t data_;

        public:
            MatWrapper() = default;
            ~MatWrapper() = default;
            MatWrapper(const Ref<const mat_t> A) : data_{A} {}

            inline mat_t& mat() { return data_; }
            inline const mat_t& mat() const { return data_; }

            inline dtype dot(const MatWrapper<dtype>& other) const {
                return (data_.conjugate().cwiseProduct(other.data_)).sum();
            }

            inline dtype dot(const SpMatWrapper<dtype>& other) const {
                // sparse * dense
                return other.spmat().cwiseProduct(data_.conjugate()).sum();
            }

            // operations
            Variable<dtype>& operator=(const Variable<dtype>& other){
                const MatWrapper* other_ptr = dynamic_cast<const MatWrapper*>(&other);
                OPTSUITE_ASSERT(other_ptr);
                data_ = other_ptr->data_;
                return *this;
            }

            inline dtype dot(const Variable<dtype>& other) const {
                const MatWrapper* other_ptr = dynamic_cast<const MatWrapper*>(&other);
                const SpMatWrapper<dtype>* other_ptr_s = dynamic_cast<const SpMatWrapper<dtype>*>(&other);
                OPTSUITE_ASSERT(other_ptr || other_ptr_s);
                if (other_ptr)
                    return this->dot(*other_ptr);
                else
                    return this->dot(*other_ptr_s);
            }

            inline void set_zero_like(const Ref<const mat_t> other) {
                data_.resize(other.rows(), other.cols());
                data_.setZero();
            }
            inline void set_zero_like(const Variable<dtype>& other) {
                const MatWrapper* other_ptr = dynamic_cast<const MatWrapper*>(&other);
                OPTSUITE_ASSERT(other_ptr);

                data_.resize(other_ptr->data_.rows(), other_ptr->data_.cols());
                data_.setZero();
            }

            inline
            virtual Scalar squared_norm_diff(const Variable<dtype>& other) const {
                const MatWrapper* other_ptr = dynamic_cast<const MatWrapper*>(&other);
                OPTSUITE_ASSERT(other_ptr);
                return (data_ - other_ptr->data_).squaredNorm();
            }

            inline
            virtual bool has_squared_norm_diff() const { return true; }

    };
}}

#endif

