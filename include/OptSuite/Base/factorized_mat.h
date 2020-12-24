/*
 * ===========================================================================
 *
 *       Filename:  factorized_mat.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/13/2020 10:11:22 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, Peking University
 *
 * ===========================================================================
 */

#ifndef OPTSUITE_BASE_FACTORIZED_MAT_H
#define OPTSUITE_BASE_FACTORIZED_MAT_H

#include <vector>
#include <memory>
#include "OptSuite/core_n.h"
#include "OptSuite/Base/variable.h"
#include "OptSuite/Base/spmat_wrapper.h"

namespace OptSuite { namespace Base {
    template<typename T> class MatWrapper;
    template<typename T> class SpMatWrapper;

    template<typename dtype>
    class FactorizedMat : public Variable<dtype> {
        using mat_t = Eigen::Matrix<dtype, Dynamic, Dynamic>;
        mat_t U_;
        mat_t V_;

        public:
            FactorizedMat() = default;
            FactorizedMat(Index m, Index n, Index k){
                U_.resize(m, k);
                V_.resize(n, k);
            }
            FactorizedMat(const Ref<const mat_t> UU, const Ref<const mat_t> VV){
                OPTSUITE_ASSERT(UU.cols() == VV.cols());
                U_ = UU;
                V_ = VV;
            }
            
            Variable<dtype>& operator=(const Variable<dtype>& other){
                const FactorizedMat* other_ptr = dynamic_cast<const FactorizedMat*>(&other);
                OPTSUITE_ASSERT(other_ptr);
                *this = *other_ptr;
                return *this;
            }

            inline
            void resize(Index m, Index n, Index k){
                U_.resize(m, k);
                V_.resize(n, k);
            }

            inline
            void set_UV(const Ref<const mat_t> UU, const Ref<const mat_t> VV){
                OPTSUITE_ASSERT(UU.cols() == VV.cols());
                U_ = UU;
                V_ = VV;
            }

            inline Index rows() const { return U_.rows(); }
            inline Index cols() const { return V_.rows(); }
            inline Index rank() const { return U_.cols(); }

            inline const mat_t& U() const { return U_; }
            inline const mat_t& V() const { return V_; }
            inline mat_t mat() const { return U_ * V_.transpose(); }

            inline dtype dot(const Variable<dtype>& other) const {
                const FactorizedMat* other_ptr_f = dynamic_cast<const FactorizedMat*>(&other);
                const SpMatWrapper<dtype>* other_ptr_s = dynamic_cast<const SpMatWrapper<dtype>*>(&other);

                OPTSUITE_ASSERT(other_ptr_f || other_ptr_s);

                if (other_ptr_f)
                    return this->dot(*other_ptr_f);
                else
                    return this->dot(*other_ptr_s);
            }

            inline dtype dot(const FactorizedMat& other) const {
                // <U1V1', U2V2'> = <U1'U2, V1'V2>
                return (U_.transpose() * other.U_).conjugate()
                    .cwiseProduct(V_.transpose() * other.V_).sum();
            }

            inline dtype dot(const SpMatWrapper<dtype>& other) const {
                // <UV', S> = <U, SV>
                return U_.conjugate().cwiseProduct(other.spmat() * V_).sum();
            }

            inline void set_zero_like(const Variable<dtype>& other){
                const FactorizedMat* other_ptr = dynamic_cast<const FactorizedMat*>(&other);
                OPTSUITE_ASSERT(other_ptr);
                set_zero_like(*other_ptr);
            }

            inline void set_zero_like(const FactorizedMat<dtype>& other){
                this->resize(other.rows(), other.cols(), other.rank());
            }
    };
}}

#endif

