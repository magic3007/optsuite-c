/*
 * ==========================================================================
 *
 *       Filename:  lansvd.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/03/2020 08:18:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#include "OptSuite/core_n.h"
#include "OptSuite/Base/mat_op.h"
#include "OptSuite/LinAlg/lansvd.h"


namespace OptSuite { namespace LinAlg {
    template<typename T>
    LANSVD<T>::LANSVD(){
        register_options();
    }

    template<typename T>
    LANSVD<T>& LANSVD<T>::compute(const Ref<const mat_t> A, int k, char which){
        // A maybe a block or a view. It is necessary to
        // consider stride information of A
        int iparam[2];
        iparam[0] = A.outerStride();
        iparam[1] = A.innerStride();
        int m = A.rows(), n = A.cols();

        // call impl
        compute_impl(m, n, k, which, aprod_dense<T>, A.data(), iparam);

        return *this;
    }

    template<typename T>
    LANSVD<T>& LANSVD<T>::compute(const Ref<const spmat_t> A, int k, char which){
        int m = A.rows(), n = A.cols();
        SparseIndex nnz = A.nonZeros();
        // convert A to raw pointers
        // in order to avoid copy, store the pointers of A to a poiner array
        // and then convert back
        // note: A must be in compressed form
        const SparseIndex * iparam[3];
        iparam[0] = &nnz;
        iparam[1] = A.outerIndexPtr();
        iparam[2] = A.innerIndexPtr();

        // call impl
        compute_impl(m, n, k, which, aprod_sparse<T>, A.valuePtr(),
                reinterpret_cast<const int*>(iparam));

        return *this;
    }

    template<typename T>
    LANSVD<T>& LANSVD<T>::compute(const Base::MatOp<T>& Aop, int k, char which){
        // extract information
        int m = Aop.rows(), n = Aop.cols();

        // call impl
        compute_impl(m, n, k, which, aprod_op<T>, NULL,
                reinterpret_cast<const int*>(&Aop));

        return *this;
    }


    template<typename T>
    void LANSVD<T>::compute_impl(int m, int n, int k, char which, APROD aprod,
            const T* dparam, const int* iparam){
        int mn_min = std::min(m, n);

        // check which parameter
        // note: which == 's' produces wrong results if kmax is small,
        //       this disabled.
        switch (which) {
            case 'l':
            case 'L':
                which = 'l';
                break;
            case 's':
            case 'S':
                Utils::Global::logger_e.log_info("Computing k smallest singular "
                        "values is not supported.\n");
                info_ = -99;
                return;
                // which = 's';
                // break;
            default:
                Utils::Global::logger_e.log_info("Invalid \'which\' parameter.\n");
                info_ = -100;
                return;
        }

        char jobu = 'y', jobv = 'y';
        // read from option
        bool is_irl = options_.get_bool("ir");
        if (!is_irl && which == 's')
            Utils::Global::logger_e.log_info("Ignoring \'which\' parameter in "
                    "non-implicit-restart mode.\n");
        int kmax = options_.get_integer("kmax");
        int maxiter = options_.get_integer("maxit");
        int nshift = options_.get_integer("nshift");
        if (kmax == -1){
            if (is_irl)
                kmax = std::min(std::max(3 * k + 1, 20), mn_min);
            else
                kmax = std::max(mn_min / 2, k);
        }
        if (nshift == -1)
            nshift = std::min(k, kmax - k);
        T tolin = options_.get_scalar("tol");

        // fix options -- kmax
        if (kmax > mn_min || kmax < k){
            Utils::Global::logger_e.log_info(
                    "Option \'kmax\' is not valid. Switch to default.\n");
            kmax = std::min(std::max(3 * k + 1, 20), mn_min);
        }

        // fix options -- nshift
        if (k + nshift > kmax){
            Utils::Global::logger_e.log_info(
                    "Option \'nshift\' is not valid. Switch to default.\n");
            nshift = std::min(k, kmax - k);
        }

        // initialize U and V
        U_.resize(m, kmax + 1);
        d_.resize(k, 1);
        V_.resize(n, kmax);
        bnd_.resize(k, 1);
        U_.setZero();
        d_.setZero();
        V_.setZero();

        int ldu = U_.rows(), ldv = V_.rows();

        // compute lwork and liwork
        int nb = 32; // how to determine the block size??
        int lwork = m + n + 13 * kmax + 8 * kmax * kmax + nb * std::max(m, n) + 8;
        int liwork = 8 * kmax;
        work.resize(lwork);
        iwork.resize(liwork);

        // options
        T doption[4] = {std::sqrt(eps), std::pow(eps, 0.75), 0.0_s, 0.002_s};
        int ioption[2] = {0, 1};
        doption[2] = options_.get_scalar("Anorm");

        if (is_irl){
            // call ?lansvd_irl_
#if OPTSUITE_SCALAR_TOKEN == 0
            dlansvd_irl_(
#elif OPTSUITE_SCALAR_TOKEN == 1
            slansvd_irl_(
#endif
                    &which,
                    &jobu,
                    &jobv,
                    &m,
                    &n,
                    &kmax,
                    &nshift,
                    &k,
                    &maxiter,
                    aprod,
                    U_.data(),
                    &ldu,
                    d_.data(),
                    bnd_.data(),
                    V_.data(),
                    &ldv,
                    &tolin,
                    work.data(),
                    &lwork,
                    iwork.data(),
                    &liwork,
                    doption,
                    ioption,
                    &info_,
                    const_cast<T*>(dparam),
                    const_cast<int*>(iparam)
                        );
        } else {
            // call ?lansvd_
#if OPTSUITE_SCALAR_TOKEN == 0
            dlansvd_(
#elif OPTSUITE_SCALAR_TOKEN == 1
            slansvd_(
#endif
                    &jobu,
                    &jobv,
                    &m,
                    &n,
                    &k,
                    &kmax,
                    aprod,
                    U_.data(),
                    &ldu,
                    d_.data(),
                    bnd_.data(),
                    V_.data(),
                    &ldv,
                    &tolin,
                    work.data(),
                    &lwork,
                    iwork.data(),
                    &liwork,
                    doption,
                    ioption,
                    &info_,
                    const_cast<T*>(dparam),
                    const_cast<int*>(iparam)
                        );
        }
    }

    template<typename T>
    void LANSVD<T>::register_options(){
        using namespace Utils;
        using OptionChecker_ptr = std::shared_ptr<OptionChecker>;
        using OptSuite::Utils::BoundCheckerSense;
        using OptSuite::eps;
        std::vector<RegOption> v;

        OptionChecker_ptr pos_scalar_checker =
            std::make_shared<BoundChecker<Scalar>>(0, BoundCheckerSense::Strict, 0, BoundCheckerSense::None);
        OptionChecker_ptr pos_eq_scalar_checker =
            std::make_shared<BoundChecker<Scalar>>(0, BoundCheckerSense::Standard, 0, BoundCheckerSense::None);

        OptionChecker_ptr dimension_checker = 
            std::make_shared<BoundChecker<Index>>(-1, BoundCheckerSense::Standard, 0, BoundCheckerSense::None);
        OptionChecker_ptr pos_int_checker = 
            std::make_shared<BoundChecker<Index>>(0, BoundCheckerSense::Strict, 0, BoundCheckerSense::None);

        v.push_back({"kmax", -1_i, "Maximal dimension of the generated Krylov subspace.",
                dimension_checker});
        v.push_back({"tol", 1e-6_s, "Desired relative accuracy of computed singular values.",
                pos_scalar_checker});
        v.push_back({"ir", true, "Whether to perform implicit restart. Default: true"});
        v.push_back({"maxit", 300_i, "Max number of iterations. Default: 300",
                pos_int_checker});
        v.push_back({"Anorm", 0.0_s, "Estimation of ||A||.",
                pos_eq_scalar_checker});
        v.push_back({"nshift", -1_i, "Number of shifts per restart.",
                dimension_checker});
        // v is constructed, now initialize options_
        this->options_ = Utils::OptionList(v);
    }

    template<typename T>
    const typename LANSVD<T>::mat_t& LANSVD<T>::U() const { return U_; }

    template<typename T>
    const typename LANSVD<T>::mat_t& LANSVD<T>::V() const { return V_; }

    template<typename T>
    const typename LANSVD<T>::vec_t& LANSVD<T>::d() const { return d_; }

    template<typename T>
    const typename LANSVD<T>::vec_t& LANSVD<T>::bnd() const { return bnd_; }

    template<typename T>
    const int& LANSVD<T>::info() const { return info_; }

    template<typename T>
    const Utils::OptionList& LANSVD<T>::options() const {
        return options_;
    }

    template<typename T>
    Utils::OptionList& LANSVD<T>::options(){
        return options_;
    }

    // instantiate
    template class LANSVD<Scalar>;

    template<typename T>
    void aprod_dense(
        char *trans,
        int *m,
        int *n,
        T *x,
        T *y,
        T *dparam,
        int *iparam){

        using namespace OptSuite;
        char trans_v = *trans;
        int m_v = *m, n_v = *n;

        // dparam contains data information
        // iparam contains stride and aligned information
        // iparam[0]: outer (lda)
        // iparam[1]: inner
        Map<Eigen::Matrix<T, Dynamic, Dynamic>, 
            Eigen::Unaligned, Stride<Dynamic, Dynamic> > A(dparam, m_v, n_v, 
                    Stride<Dynamic, Dynamic>(iparam[0], iparam[1]));

        if (trans_v == 'n' || trans_v == 'N'){
            Map<Eigen::Matrix<T, Dynamic, 1>> X(x, n_v);
            Map<Eigen::Matrix<T, Dynamic, 1>> Y(y, m_v);

            Y = A * X;

        }
        if (trans_v == 't' || trans_v == 'T'){
            Map<Eigen::Matrix<T, Dynamic, 1>> X(x, m_v);
            Map<Eigen::Matrix<T, Dynamic, 1>> Y(y, n_v);

            Y = A.transpose() * X;

        }
    }

    template<typename T>
    void aprod_sparse(
            char *trans,
            int *m,
            int *n,
            T *x,
            T *y,
            T *dparam,
            int *iparam){

        using namespace OptSuite;
        char trans_v = *trans;
        int m_v = *m, n_v = *n;
        SparseIndex ** iparam_orig = reinterpret_cast<SparseIndex **>(iparam);

        // dparam contains data information
        // iparam contains non-zeros and colptr and rowind
        // iparam[0] - ptr of nnz
        // iparam[1] - colptr
        // iparam[2] - rowind
        Map<Eigen::SparseMatrix<T, ColMajor, SparseIndex>>
            A(m_v, n_v, *iparam_orig[0], iparam_orig[1], iparam_orig[2], dparam);

        if (trans_v == 'n' || trans_v == 'N'){
            Map<Eigen::Matrix<T, Dynamic, 1>> X(x, n_v);
            Map<Eigen::Matrix<T, Dynamic, 1>> Y(y, m_v);

            Y = A * X;

        }
        if (trans_v == 't' || trans_v == 'T'){
            Map<Eigen::Matrix<T, Dynamic, 1>> X(x, m_v);
            Map<Eigen::Matrix<T, Dynamic, 1>> Y(y, n_v);

            Y = A.transpose() * X;

        }
    }

    template<typename T>
    void aprod_op(
        char *trans,
        int *m,
        int *n,
        T *x,
        T *y,
        T *,
        int *iparam){

        using namespace OptSuite;
        using namespace OptSuite::Base;
        char trans_v = *trans;
        int m_v = *m, n_v = *n;

        // iparam contains pointer to op
        // dparam is ignored
        MatOp<T>* iparam_orig = reinterpret_cast<MatOp<T>*>(iparam);
        MatOp<T>& A = *iparam_orig;

        if (trans_v == 'n' || trans_v == 'N'){
            Map<Eigen::Matrix<T, Dynamic, 1>> X(x, n_v);
            Map<Eigen::Matrix<T, Dynamic, 1>> Y(y, m_v);

            A.apply(X, Y);
        }
        if (trans_v == 't' || trans_v == 'T'){
            Map<Eigen::Matrix<T, Dynamic, 1>> X(x, m_v);
            Map<Eigen::Matrix<T, Dynamic, 1>> Y(y, n_v);

            A.apply_transpose(X, Y);
        }
    }

}}
