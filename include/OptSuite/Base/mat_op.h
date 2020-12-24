/*
 * ===========================================================================
 *
 *       Filename:  mat_op.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/13/2020 10:38:44 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, Peking University
 *
 * ===========================================================================
 */

#ifndef OPTSUITE_BASE_MAT_OP_H
#define OPTSUITE_BASE_MAT_OP_H

#include "OptSuite/core_n.h"
#include "OptSuite/Base/spmat_wrapper.h"
#include "OptSuite/Base/factorized_mat.h"

namespace OptSuite { namespace Base {
    template<typename dtype>
    class MatOp {
        using mat_t = Eigen::Matrix<dtype, Dynamic, Dynamic>;
        Index rows_;
        Index cols_;
        public:
            MatOp() = default;
            MatOp(Index m, Index n) : rows_(m), cols_(n) {}

            inline void resize(Index m, Index n){
                rows_ = m;
                cols_ = n;
            }

            inline Index rows() const { return rows_; }
            inline Index cols() const { return cols_; }

            // virtual functions
            virtual void apply(const Ref<const mat_t>, Ref<mat_t>) const = 0;
            virtual void apply_transpose(const Ref<const mat_t>, Ref<mat_t>) const = 0;
    };

    template<typename dtype>
    class FactorizePSpMatOp : public MatOp<dtype> {
        using mat_t = Eigen::Matrix<dtype, Dynamic, Dynamic>;
        Scalar tau;
        const FactorizedMat<dtype>* mat_F;
        const SpMatWrapper<dtype>* mat_S;

        public:
            FactorizePSpMatOp() : mat_F(NULL), mat_S(NULL), tau(0_s) {}
            FactorizePSpMatOp(Index m, Index n) : MatOp<dtype>(m, n) {}
            FactorizePSpMatOp(const FactorizedMat<dtype>& mf, Scalar t, const SpMatWrapper<dtype>& ms)
                : MatOp<dtype>(mf.rows(), mf.cols()), tau(t), mat_F(&mf), mat_S(&ms) {}

            inline void apply(const Ref<const mat_t> in, Ref<mat_t> out) const {
                OPTSUITE_ASSERT(mat_F && mat_S);
                // (UV' + tauS)X = U(V'X) + tau (SX)
                if (tau == 0_s)
                    out = mat_F->U() * (mat_F->V().transpose() * in);
                else
                    out = mat_F->U() * (mat_F->V().transpose() * in) +
                        tau * (mat_S->spmat() * in);
            }

            inline void apply_transpose(const Ref<const mat_t> in, Ref<mat_t> out) const {
                OPTSUITE_ASSERT(mat_F && mat_S);
                // (UV' + tauS)'X = V(U'X) + tau (S'X)
                if (tau == 0_s)
                    out = mat_F->V() * (mat_F->U().transpose() * in);
                else
                    out = mat_F->V() * (mat_F->U().transpose() * in) +
                        tau * (mat_S->spmat().transpose() * in);
            }
    };
}}

#endif

