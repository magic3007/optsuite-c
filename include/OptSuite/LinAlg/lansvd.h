/*
 * ==========================================================================
 *
 *       Filename:  lansvd.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/03/2020 06:37:11 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#ifndef OPTSUITE_LINALG_LANSVD_H
#define OPTSUITE_LINALG_LANSVD_H

#include <vector>
#include "OptSuite/core_n.h"
#include "OptSuite/Base/mat_op.h"
#include "OptSuite/Utils/optionlist.h"
#include "OptSuite/Utils/optionchecker.h"

extern "C" {
    typedef void (*DAPROD)(
            char *,
            int *,
            int *,
            double *,
            double *,
            double *,
            int *
            );
    typedef void (*SAPROD)(
            char *,
            int *,
            int *,
            float *,
            float *,
            float *,
            int *
            );

    extern void dlansvd_(
            char *jobu,
            char *jobv,
            int *m,
            int *n,
            int *k,
            int *kmax,
            DAPROD aprod,
            double *U,
            int *ldu,
            double *Sigma,
            double *bnd,
            double *V,
            int *ldv,
            double *tolin,
            double *work,
            int *lwork,
            int *iwork,
            int *liwork,
            double *doption,
            int *ioption,
            int *info,
            double *dparam,
            int *iparam
                );

    extern void dlansvd_irl_(
            char *which,
            char *jobu,
            char *jobv,
            int *m,
            int *n,
            int *dim,
            int *p,
            int *k,
            int *maxiter,
            DAPROD aprod,
            double *U,
            int *ldu,
            double *Sigma,
            double *bnd,
            double *V,
            int *ldv,
            double *tolin,
            double *work,
            int *lwork,
            int *iwork,
            int *liwork,
            double *doption,
            int *ioption,
            int *info,
            double *dparam,
            int *iparam
                );

    extern void slansvd_(
            char *jobu,
            char *jobv,
            int *m,
            int *n,
            int *k,
            int *kmax,
            SAPROD aprod,
            float *U,
            int *ldu,
            float  *Sigma,
            float *bnd,
            float *V,
            int *ldv,
            float *tolin,
            float *work,
            int *lwork,
            int *iwork,
            int *liwork,
            float *soption,
            int *ioption,
            int *info,
            float *sparam,
            int *iparam
                );

    extern void slansvd_irl_(
            char *which,
            char *jobu,
            char *jobv,
            int *m,
            int *n,
            int *dim,
            int *p,
            int *k,
            int *maxiter,
            SAPROD aprod,
            float *U,
            int *ldu,
            float *Sigma,
            float *bnd,
            float *V,
            int *ldv,
            float *tolin,
            float *work,
            int *lwork,
            int *iwork,
            int *liwork,
            float *soption,
            int *ioption,
            int *info,
            float *sparam,
            int *iparam
                );

}

namespace OptSuite { namespace LinAlg {
    template <typename T>
    class LANSVD {
        using mat_t = Eigen::Matrix<T, Dynamic, Dynamic>;
        using vec_t = Eigen::Matrix<T, Dynamic, 1>;
        using spmat_t = Eigen::SparseMatrix<Scalar, ColMajor, SparseIndex>;
        typedef void (*APROD)(
            char *,
            int *,
            int *,
            T *,
            T *,
            T *,
            int *
            );


        std::vector<T> work;
        std::vector<int> iwork;

        mat_t U_;
        vec_t d_;
        mat_t V_;
        vec_t bnd_;
        int info_;

        Utils::OptionList options_;
        void register_options();
        void compute_impl(int, int, int, char, APROD, const T*, const int*);

        public:
            LANSVD();
            LANSVD& compute(const Ref<const mat_t>, int, char = 'l');
            LANSVD& compute(const Ref<const spmat_t>, int, char = 'l');
            LANSVD& compute(const Base::MatOp<T>&, int, char = 'l');
            const mat_t& U() const;
            const mat_t& V() const;
            const vec_t& d() const;
            const vec_t& bnd() const;
            const int& info() const;

            const Utils::OptionList& options() const;
            Utils::OptionList& options();
    };

    template<typename T>
    void aprod_dense(
            char *,
            int *,
            int *,
            T *,
            T *,
            T *,
            int *
            );

    template<typename T>
    void aprod_sparse(
            char *,
            int *,
            int *,
            T *,
            T *,
            T *,
            int *
            );

    template<typename T>
    void aprod_op(
            char *,
            int *,
            int *,
            T *,
            T *,
            T *,
            int *
            );

}}

#endif
