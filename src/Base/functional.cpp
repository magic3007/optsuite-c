/*
 * ==========================================================================
 *
 *       Filename:  functional.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/14/2020 07:02:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#include <cstring>
#include "OptSuite/core_n.h"
#include "OptSuite/Base/functional.h"
#include "OptSuite/Base/mat_op.h"
#include "OptSuite/Utils/tictoc.h"

namespace OptSuite { namespace Base {
    template<typename dtype>
    Scalar Func<dtype>::operator()(const Ref<const mat_t>){
        OPTSUITE_ASSERT(0);
        return 0;
    }

    template<typename dtype>
    Scalar Func<dtype>::operator()(const mat_wrapper_t& mw){
        return (*this)(mw.mat());
    }

    template<typename dtype>
    Scalar Func<dtype>::operator()(const mat_array_t& ma){
        Scalar r = 0;
        for (const auto& i : ma)
            r += (*this)(i);
        return r;
    }

    template<typename dtype>
    Scalar Func<dtype>::operator()(const var_t& var){
        const mat_wrapper_t* var_ptr = dynamic_cast<const mat_wrapper_t*>(&var);
        OPTSUITE_ASSERT(var_ptr);
        return (*this)(*var_ptr);
    }

    template<typename dtype>
    void Proximal<dtype>::operator()(const Ref<const mat_t>, Scalar, Ref<mat_t>){
        OPTSUITE_ASSERT(0);
    }

    template<typename dtype>
    void Proximal<dtype>::operator()(const mat_wrapper_t& mw, Scalar v, mat_wrapper_t& mw_out){
        (*this)(mw.mat(), v, mw_out.mat());
    }

    template<typename dtype>
    void Proximal<dtype>::operator()(const mat_array_t& ma, Scalar v, mat_array_t& ma_out){
        for (int i = 0; i < ma.total_blocks(); ++i)
            (*this)(ma[i], v, ma_out[i]);
    }

    template<typename dtype>
    void Proximal<dtype>::operator()(const var_t& xp, Scalar tau, const var_t& gp,
                                     Scalar v, var_t& x){
        const mat_wrapper_t* xp_ptr = dynamic_cast<const mat_wrapper_t*>(&xp);
        const mat_wrapper_t* gp_ptr = dynamic_cast<const mat_wrapper_t*>(&gp);
              mat_wrapper_t*  x_ptr = dynamic_cast<mat_wrapper_t*>(&x);

        OPTSUITE_ASSERT(xp_ptr != NULL && gp_ptr != NULL && x_ptr != NULL);
        // x_tmp is created on every call of operator()
        mat_wrapper_t x_tmp;
        x_tmp.set_zero_like(*xp_ptr);
        x_tmp.mat() = xp_ptr->mat() - tau * gp_ptr->mat();
        (*this)(x_tmp, v, *x_ptr);
    }

    // template instantiation
    template class Proximal<Scalar>;
    template class Func<Scalar>;
    template class Proximal<ComplexScalar>;
    template class Func<ComplexScalar>;

    Scalar L1Norm::operator()(const Ref<const mat_t> x){
        return mu * x.lpNorm<1>();
    }


    Scalar L2Norm::operator()(const Ref<const mat_t> x){
        return mu * x.norm();
    }

    Scalar L1_2Norm::operator()(const Ref<const mat_t> x){
        return mu * x.rowwise().norm().sum();
    }

    Scalar NuclearNorm::operator()(const Ref<const mat_t> x){
        using Eigen::DecompositionOptions;
        svd.compute(x);
        return mu * svd.singularValues().sum();
    }

    Scalar NuclearNorm::operator()(const fmat_t& x){
        Index r = x.rank();
        Mat R_U = qr.compute(x.U()).matrixQR().topRows(r).triangularView<Eigen::Upper>();
        Mat R_V = qr.compute(x.V()).matrixQR().topRows(r).triangularView<Eigen::Upper>();

        svd.compute(R_U * R_V.transpose());
        return mu * svd.singularValues().sum();
    }

    Scalar NuclearNorm::operator()(const var_t& x){
        const mat_wrapper_t* x_ptr = dynamic_cast<const mat_wrapper_t*>(&x);
        const fmat_t* x_ptr_f = dynamic_cast<const fmat_t*>(&x);

        OPTSUITE_ASSERT(x_ptr || x_ptr_f);

        if (x_ptr)
            return (*this)(x_ptr->mat());
        else
            return (*this)(*x_ptr_f);
    }

    void ShrinkageL1::operator()(const Ref<const mat_t> x, Scalar t, Ref<mat_t> y){
        y.array() = x.array().sign() * (x.array().abs() - t * mu).max(0);
    }

    void ShrinkageL2::operator()(const Ref<const mat_t> x, Scalar t, Ref<mat_t> y) {
        Scalar lambda = 1 - t * mu / x.norm();
        y.array()     = x.array() * std::max(0_s, lambda);
    }

    void ShrinkageL2Rowwise::operator()(const Ref<const mat_t> x, Scalar t, Ref<mat_t> y){
        lambda = 1 - t * mu / x.rowwise().norm().array();
        y = x.array().colwise() * lambda.array().max(0);
    }

    Index ShrinkageNuclear::compute_rank() const {
        // find the first index < threshold
        // using binary search algorithm
        Index lo = 0_i, hi = d.size() - 1_i, i;
        while ( lo < hi ){
            i = (hi + lo) / 2_i;
            if (d[i] < threshold)
                hi = i;
            else // d[i] > threshold
                lo = i + 1_i;
        }

        return d[lo] >= threshold ? lo + 1_i : lo;
    }

    void ShrinkageNuclear::operator()(const Ref<const mat_t> x, Scalar t, Ref<mat_t> y){
        const vec_t& sv = svd.compute(x, op).singularValues();
        const mat_t& U = svd.matrixU();
        const mat_t& V = svd.matrixV();

        d.array() = (sv.array() - t * mu).max(0);

        Index rank = compute_rank();
        if (rank == 0)
            y = mat_t::Zero(x.rows(), x.cols());
        else
            y = (U.array().block(0, 0, x.rows(), rank).rowwise() *
                d.array().block(0, 0, rank, 1).transpose()).matrix() *
                V.block(0, 0, x.cols(), rank).transpose();
    }

    void ShrinkageNuclear::operator()(const fmat_t& xp, Scalar tau, const smat_t& gp,
            Scalar v, fmat_t& x){
        // construct mat op
        FactorizePSpMatOp<Scalar> Aop(xp, -tau, gp);

        // now add some guard vectors
        Index p = std::min(Index(xp.rank() * 1.2_s), std::min(xp.rows(), xp.cols()));
        const vec_t& sv = lansvd.compute(Aop, p).d();
        const mat_t& U = lansvd.U();
        const mat_t& V = lansvd.V();

        d.array() = (sv.array() - v * mu).max(0);

        // note: rank can be larger or smaller than xp.rank()
        // thus x.rank() can be adjusted automatically
        Index rank = compute_rank();

        x.set_UV(
                (U.array().block(0, 0, x.rows(), rank).rowwise() * d.array().block(0, 0, rank, 1).transpose().sqrt()).matrix(),
                (V.array().block(0, 0, x.cols(), rank).rowwise() * d.array().block(0, 0, rank, 1).transpose().sqrt()).matrix()
                );

    }

    void ShrinkageNuclear::operator()(const var_t& xp, Scalar tau, const var_t& gp,
            Scalar v, var_t& x){
        const mat_wrapper_t* xp_ptr = dynamic_cast<const mat_wrapper_t*>(&xp);
        const fmat_t* xp_ptr_f = dynamic_cast<const fmat_t*>(&xp);
        const mat_wrapper_t* gp_ptr = dynamic_cast<const mat_wrapper_t*>(&gp);
        const spmat_wrapper_t* gp_ptr_s = dynamic_cast<const spmat_wrapper_t*>(&gp);
              mat_wrapper_t*  x_ptr = dynamic_cast<mat_wrapper_t*>(&x);
              fmat_t* x_ptr_f = dynamic_cast<fmat_t*>(&x);

        // only the following combination is supported:
        // 1) dense + dense -> 0b0101
        // 2) dense + sparse -> 0b1001
        // 3) factor + sparse -> 0b1010
        bool is_dense_x = xp_ptr && x_ptr;
        bool is_factor_x = xp_ptr_f && x_ptr_f;
        bool is_dense_g = gp_ptr != NULL;
        bool is_sparse_g = gp_ptr_s != NULL;

        if (is_dense_x && (is_dense_g || is_sparse_g)) {
            // x_tmp is created on every call of operator()
            mat_wrapper_t x_tmp;
            x_tmp.set_zero_like(*xp_ptr);
            if (is_dense_g){
                x_tmp.mat() = xp_ptr->mat() - tau * gp_ptr->mat();
            } else {
                x_tmp.mat() = xp_ptr->mat();
                x_tmp.mat() -= tau * gp_ptr_s->spmat();
            }
            (*this)(x_tmp.mat(), v, x_ptr->mat());
        }
        else if (is_factor_x && is_sparse_g){
            (*this)(*xp_ptr_f, tau, *gp_ptr_s, v, *x_ptr_f);
        }
    }

    Scalar ShrinkageNuclear::cached_objective() const { return mu * d.array().sum(); }

    template<typename dtype>
    void L0NormBallProj<dtype>::operator()(Ref<const mat_t> x, Scalar t, Ref<mat_t> y){
        OPTSUITE_ASSERT(x.cols() == 1);
        SparseIndex count = std::floor(t * mu_);
        std::vector<SparseIndex> indexes(x.rows());
        std::iota(indexes.begin(), indexes.end(), 0);
        std::sort(indexes.begin(), indexes.end(), [&x](SparseIndex a, SparseIndex b){
            return std::fabs(x[a]) > std::fabs(x[b]);
        });
        y = x;
        for(SparseIndex i = count; i < indexes.size(); i++){
            y[indexes[i]] = 0;
        }
    }

    template<typename dtype>
    void L2NormBallProj<dtype>::operator()(Ref<const mat_t> x, Scalar t, Ref<mat_t> y) {
        y = x.array() * (t * mu_ / std::max(t * mu_, x.norm()));
    }

    template<typename dtype>
    void LInfBallProj<dtype>::operator()(Ref<const mat_t> x, Scalar t, Ref<mat_t> y) {
        y = x.array().sgn() * std::max(x.array().abs(), t * mu_);
    }

    template<typename dtype>
    Scalar FuncGrad<dtype>::operator()(const Ref<const mat_t> x) {
        mat_t        dummy_y;
        return this->operator()(x, dummy_y, false);
    }

    template<typename dtype>
    Scalar FuncGrad<dtype>::operator()(const Ref<const mat_t>, Ref<mat_t>, bool){
        OPTSUITE_ASSERT(0);
        return 0;
    }

    template<typename dtype>
    Scalar FuncGrad<dtype>::operator()(const mat_wrapper_t& x){
        mat_wrapper_t dummy_y;
        return this->operator()(x, dummy_y, false);
    }

    template<typename dtype>
    Scalar FuncGrad<dtype>::operator()(const mat_wrapper_t& x, mat_wrapper_t& y, bool compute_grad){
        return this->operator()(x.mat(), y.mat(), compute_grad);
    }



    template<typename dtype>
    Scalar FuncGrad<dtype>::operator()(const mat_array_t& ma){
        Scalar r = 0;
        for(const auto& i : ma)
            r += (*this)(i);
        return r;
    }

    template<typename dtype>
    Scalar FuncGrad<dtype>::operator()(const mat_array_t& x, mat_array_t& y, bool compute_grad){
        Scalar r = 0;
        for(int i = 0; i < x.total_blocks(); ++i)
            r += (*this)(x[i], y[i], compute_grad);
        return r;
    }

    template<typename dtype>
    Scalar FuncGrad<dtype>::operator()(const var_t& x){
        mat_wrapper_t dummy_y;
        return (*this)(x, dummy_y, false);
    }

    template<typename dtype>
    Scalar FuncGrad<dtype>::operator()(const var_t& x, var_t& y, bool compute_grad){
        const mat_wrapper_t* x_ptr = dynamic_cast<const mat_wrapper_t*>(&x);
              mat_wrapper_t* y_ptr = dynamic_cast<mat_wrapper_t*>(&y);

        OPTSUITE_ASSERT(x_ptr != NULL && y_ptr != NULL);
        return (*this)(*x_ptr, *y_ptr, compute_grad);
    }

    template<typename dtype>
    AxmbNormSqr<dtype>::AxmbNormSqr(const Ref<const mat_t> A, const Ref<const mat_t> b){
        OPTSUITE_ASSERT(A.rows() == b.rows());
        this->A = A;
        this->b = b;
    }

    template<typename dtype>
    Scalar AxmbNormSqr<dtype>::operator()(const Ref<const mat_t> x, Ref<mat_t> y, bool compute_grad){
        r.noalias() = A * x - b;
        Scalar fun = 0.5 * r.squaredNorm();
        if (compute_grad) y.noalias() = A.transpose() * r;
        return fun;
    }

    template<typename dtype>
    const typename AxmbNormSqr<dtype>::mat_t& AxmbNormSqr<dtype>::get_A() const {
        return A;
    }

    template<typename dtype>
    const typename AxmbNormSqr<dtype>::mat_t &AxmbNormSqr<dtype>::get_b() const {
        return b;
    }

    // instantiate
    template class AxmbNormSqr<Scalar>;
    template class AxmbNormSqr<ComplexScalar>;

    template<typename dtype>
    LogisticRegression<dtype>::LogisticRegression(Ref<const mat_t> A, Ref<const mat_t> b)
        : A_(A), b_(b) {
        OPTSUITE_ASSERT(b.cols() == 1);
        OPTSUITE_ASSERT(b.rows() == A.cols());
        mbA_ = A_.array().rowwise() * (- b_.transpose().array());
    }

    template<typename dtype>
    Scalar LogisticRegression<dtype>::operator()(Ref<const mat_t> x, Ref<mat_t> y,
                                                 bool compute_grad) {
        OPTSUITE_ASSERT(x.cols() == 1);
        OPTSUITE_ASSERT(x.rows() == A_.rows());
        col_vec_t t   = mbA_.transpose() * x;
        col_vec_t p   = t.array().exp();
        col_vec_t q   = p.array() + 1;
        col_vec_t r   = q.array().log();
        Scalar    fun = r.mean();
        if (compute_grad) {
            col_vec_t s = p.array() / q.array();
            mat_t     u = mbA_.array().rowwise() * s.transpose().array();
            y           = u.array().rowwise().mean();
        }
        return fun;
    }
    template class LogisticRegression<Scalar>;

    template<typename dtype>
    ProjectionOmega<dtype>::ProjectionOmega(const Ref<const spmat_t> ref,
                                            const Ref<const mat_t>   b_) {
        outerIndexPtr.resize(ref.cols() + 1_i);
        innerIndexPtr.resize(ref.nonZeros());

        std::memcpy(outerIndexPtr.data(), ref.outerIndexPtr(),
                    sizeof(SparseIndex) * outerIndexPtr.size());
        std::memcpy(innerIndexPtr.data(), ref.innerIndexPtr(),
                    sizeof(SparseIndex) * innerIndexPtr.size());

        b = b_;
    }

    template<typename dtype>
    Scalar ProjectionOmega<dtype>::operator()(const Ref<const mat_t> x, Ref<mat_t> y, bool compute_grad){
        // compute r = P(x)
        projection(x);
        r -= b;
        Scalar fun = 0.5_s * r.squaredNorm();

        if (compute_grad){
            // note: the gradient is sparse, convert it to dense
            y = mat_t(Map<spmat_t>(x.rows(), x.cols(), r.size(),
                        outerIndexPtr.data(), innerIndexPtr.data(), r.data()));
        }

        return fun;
    }

    template<typename dtype>
    Scalar ProjectionOmega<dtype>::operator()(const var_t& x, var_t& y, bool compute_grad){
        const MatWrapper<dtype>* x_ptr = dynamic_cast<const MatWrapper<dtype>*>(&x);
        const fmat_t* x_ptr_f = dynamic_cast<const fmat_t*>(&x);
        MatWrapper<dtype>* y_ptr = dynamic_cast<MatWrapper<dtype>*>(&y);
        SpMatWrapper<dtype>* y_ptr_s = dynamic_cast<SpMatWrapper<dtype>*>(&y);
        Index m, n;

        if (x_ptr && y_ptr) // dense x + dense y
            return (*this)(x_ptr->mat(), y_ptr->mat(), compute_grad);
        else if ((x_ptr_f || x_ptr) && (!compute_grad || y_ptr_s)) { // dense/factorized x + sparse y
            // compute r = P(x)
            if (x_ptr){ // dense
                m = x_ptr->mat().rows();
                n = x_ptr->mat().cols();
                projection(x_ptr->mat());
            } else { // factor
                m = x_ptr_f->rows();
                n = x_ptr_f->cols();
                projection(*x_ptr_f);
            }

            r -= b;
            Scalar fun = 0.5_s * r.squaredNorm();

            // compute gradient
            // note: the gradient is sparse
            if (compute_grad){
                // if y_ptr_s isn't initialized, set zeros as default
                if (y_ptr_s->spmat().nonZeros() != b.rows())
                    y_ptr_s->set_zero_like(m, n, r.rows(),
                            outerIndexPtr.data(), innerIndexPtr.data());

                std::memcpy(y_ptr_s->spmat().valuePtr(), r.data(), r.rows() * sizeof(dtype));
            }
            return fun;
        } else {
            OPTSUITE_ASSERT(0);
            return 0;
        }
    }

    template<typename dtype>
    void ProjectionOmega<dtype>::projection(const Ref<const mat_t> x) {
        r.resize(b.rows(), 1);
        SparseIndex *outer_ptr = outerIndexPtr.data();
        SparseIndex *inner_ptr = innerIndexPtr.data();
        Scalar *     r_ptr     = r.data();

        for (size_t i = 0; i < outerIndexPtr.size() - 1; ++i) {
            for (Index j = outer_ptr[i]; j < outer_ptr[i + 1]; ++j) {
                *r_ptr++ = x(inner_ptr[j], i);   // for colmajor
            }
        }
    }

    template<typename dtype>
    void ProjectionOmega<dtype>::projection(const fmat_t& x){
        r.resize(b.rows(), 1);
        SparseIndex *outer_ptr = outerIndexPtr.data();
        SparseIndex *inner_ptr = innerIndexPtr.data();
        Scalar *r_ptr = r.data();

        for (size_t i = 0; i < outerIndexPtr.size() - 1; ++i){
            for (Index j = outer_ptr[i]; j < outer_ptr[i+1]; ++j){
                *r_ptr++ = x.U().row(inner_ptr[j]).dot(x.V().row(i));
            }
        }
    }


    // instantiate
    template class ProjectionOmega<Scalar>;
}}
