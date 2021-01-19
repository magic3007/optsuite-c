/*
 * ==========================================================================
 *
 *       Filename:  functional.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/04/2020 05:07:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */
#ifndef OPTSUITE_BASE_FUNCTIONAL_H
#define OPTSUITE_BASE_FUNCTIONAL_H

#include "OptSuite/core_n.h"
#include "OptSuite/Base/variable.h"
#include "OptSuite/Base/mat_wrapper.h"
#include "OptSuite/Base/mat_array.h"
#include "OptSuite/Base/spmat_wrapper.h"
#include "OptSuite/Base/factorized_mat.h"
#include "OptSuite/LinAlg/lansvd.h"

namespace OptSuite { namespace Base {
    class Functional {
        public:
            Functional() = default;
            ~Functional() = default;

        private:
    };

    template<typename dtype>
    class Func : public Functional {
        public:
            using mat_t = Eigen::Matrix<dtype, Dynamic, Dynamic>;
            using mat_wrapper_t = MatWrapper<dtype>;
            using mat_array_t = MatArray_t<dtype>;
            using var_t = Variable<dtype>;
            using fmat_t = FactorizedMat<dtype>;
            Func() = default;
            ~Func() = default;

            virtual Scalar operator()(const Ref<const mat_t>);
            virtual Scalar operator()(const mat_wrapper_t&);
            virtual Scalar operator()(const mat_array_t&);
            virtual Scalar operator()(const var_t&);
    };

    template<typename dtype>
    class Proximal : public Functional {
    public:
        using mat_t           = Eigen::Matrix<dtype, Dynamic, Dynamic>;
        using mat_wrapper_t   = MatWrapper<dtype>;
        using spmat_wrapper_t = SpMatWrapper<dtype>;
        using mat_array_t     = MatArray_t<dtype>;
        using var_t           = Variable<dtype>;
        using fmat_t          = FactorizedMat<dtype>;
        ~Proximal()           = default;

        virtual void   operator()(const Ref<const mat_t>, Scalar, Ref<mat_t>);
        virtual void   operator()(const mat_wrapper_t &, Scalar, mat_wrapper_t &);
        virtual void   operator()(const mat_array_t &, Scalar, mat_array_t &);
        virtual void   operator()(const var_t &, Scalar, const var_t &, Scalar, var_t &);
        virtual bool   is_identity() const { return false; }
        virtual bool   has_objective_cache() const { return false; }
        virtual Scalar cached_objective() const { return 0_s; }
    };

    template<typename dtype>
    class Grad : public Functional {
        public:
            using mat_t = Eigen::Matrix<dtype, Dynamic, Dynamic>;
            Grad() = default;
            ~Grad() = default;

            virtual void operator()(const Ref<const mat_t>, Ref<mat_t>) = 0;

        protected:
            unsigned num_eval = 0;
    };

    template<typename dtype = Scalar>
    class Zero : public Func<dtype> {
        using typename Func<dtype>::mat_t;
        public:
            Scalar operator()(const Ref<const mat_t>){
                return 0_s;
            }
    };

    
    class L1Norm : public Func<Scalar> {
        public:
            inline L1Norm(Scalar mu_ = 1) : mu(mu_) {}
            Scalar operator()(const Ref<const mat_t>);

            Scalar mu;
    };

    class L2Norm : public Func<Scalar> {
        public:
            inline L2Norm(Scalar mu_ = 1) : mu(mu_) {}
            Scalar operator()(const Ref<const mat_t>);

            Scalar mu;
    };

    class LInfNorm : public Func<Scalar> {
        public:
            inline LInfNorm(Scalar mu_ = 1) : mu(mu_) {}
            Scalar operator()(const Ref<const mat_t>);

            Scalar mu;
    };

    class L1_2Norm : public Func<Scalar> {
        public:
            inline L1_2Norm(Scalar mu_ = 1) : mu(mu_) {}
            Scalar operator()(const Ref<const mat_t>);

            Scalar mu;
    };

    class NuclearNorm : public Func<Scalar> {
        Eigen::JacobiSVD<Mat>     svd;
        Eigen::HouseholderQR<Mat> qr;

    public:
        inline NuclearNorm(Scalar mu_ = 1) : mu(mu_) {}
        Scalar operator()(const Ref<const mat_t>);
        Scalar operator()(const fmat_t &);
        Scalar operator()(const var_t &);


        Scalar mu;
    };

    class NuclearNormProx : public Proximal<Scalar> {
        Eigen::JacobiSVD<Mat>     svd;
        Eigen::HouseholderQR<Mat> qr;
        using typename Proximal<Scalar>::mat_t;
        using typename Proximal<Scalar>::fmat_t;
        using typename Proximal<Scalar>::var_t;

    public:
        explicit NuclearNormProx(Scalar mu) : mu_(mu) {}
        void operator()(Ref<const mat_t> x, Scalar t, Ref<mat_t> y);

    private:
        Scalar mu_;
    };

    template<typename dtype>
    class IdentityProx : public Proximal<dtype> {
        using typename Proximal<dtype>::mat_t;
        using typename Proximal<dtype>::mat_array_t;
        using typename Proximal<dtype>::mat_wrapper_t;

    public:
        IdentityProx()  = default;
        ~IdentityProx() = default;

        void operator()(const Ref<const mat_t> x, dtype, Ref<mat_t> y) {
            // simple copy
            y = x;
        }

        void operator()(const Variable<dtype> &xp, Scalar tau, const Variable<dtype> &gp, Scalar,
                        Variable<dtype> &x) {
            const mat_wrapper_t *xp_ptr = dynamic_cast<const mat_wrapper_t *>(&xp);
            const mat_wrapper_t *gp_ptr = dynamic_cast<const mat_wrapper_t *>(&gp);
            mat_wrapper_t *      x_ptr  = dynamic_cast<mat_wrapper_t *>(&x);

            OPTSUITE_ASSERT(xp_ptr || gp_ptr || x_ptr);
            x_ptr->mat() = xp_ptr->mat() - tau * gp_ptr->mat();
        }
        bool is_identity() const { return true; }
    };

    template<typename dtype>
    class L1NormBallProj : public Proximal<dtype> {
        using typename Proximal<dtype>::mat_t;

    public:
        explicit L1NormBallProj(Scalar mu) : mu_(mu) {}
        ~L1NormBallProj() = default;

        void operator()( Ref<const mat_t> x, Scalar t, Ref<mat_t> y);

    private:
        Scalar mu_;
    };

    template<typename dtype>
    class L0NormBallProj : public Proximal<dtype> {
    public:
        using typename Proximal<dtype>::mat_t;
        explicit L0NormBallProj(Scalar mu) : mu_(mu) {}
        ~L0NormBallProj() = default;
        void operator()( Ref<const mat_t> x, Scalar t, Ref<mat_t> y);

    private:
        Scalar mu_;
    };

    


    template<typename dtype>
    class L2NormBallProj : public Proximal<dtype> {
        using typename Proximal<dtype>::mat_t;

    public:
        explicit L2NormBallProj(Scalar mu) : mu_(mu) {}
        ~L2NormBallProj() = default;

        void operator()( Ref<const mat_t> x, Scalar t, Ref<mat_t> y);

    private:
        Scalar mu_;
    };


    template<typename dtype>
    class LInfBallProj : public Proximal<dtype> {
        using typename Proximal<dtype>::mat_t;

    public:
        explicit LInfBallProj(Scalar mu) : mu_(mu) {}
        ~LInfBallProj() = default;
        void operator()( Ref<const mat_t> x, Scalar t, Ref<mat_t> y);

    private:
        Scalar mu_;
    };

    class ShrinkageL1 : public Proximal<Scalar> {
    public:
        inline ShrinkageL1(Scalar mu_ = 1) : mu(mu_) {}
        ~ShrinkageL1() = default;

        void operator()(const Ref<const mat_t>, Scalar, Ref<mat_t>);

        Scalar mu;
    };

    class ShrinkageL2 : public Proximal<Scalar> {
    public:
        inline ShrinkageL2(Scalar mu_ = 1) : mu(mu_) {}
        ~ShrinkageL2() = default;

        void   operator()(const Ref<const mat_t>, Scalar, Ref<mat_t>);
        Scalar mu;
    };

    class ShrinkageL0 : public Proximal<Scalar> {
    public:
        ShrinkageL0(Scalar mu = 1) : mu_(mu) {}
        ~ShrinkageL0() = default;
        void operator()(Ref<const mat_t> x, Scalar t, Ref<mat_t> y);

    private:
        Scalar mu_;
    };

    class ShrinkageLInf : public Proximal<Scalar> {
    public:
        ShrinkageLInf(Scalar mu = 1) : mu_(mu) {}
        ~ShrinkageLInf() = default;
        void operator()(Ref<const mat_t> x, Scalar t, Ref<mat_t> y);

    private:
        Scalar mu_;
    };

    class ShrinkageL2Rowwise : public Proximal<Scalar> {
        using vec_t = Vec;
        vec_t lambda;

    public:
        inline ShrinkageL2Rowwise(Scalar mu_ = 1) : mu(mu_) {}
        ~ShrinkageL2Rowwise() = default;

        void operator()(const Ref<const mat_t>, Scalar, Ref<mat_t>);

        Scalar mu;
    };

    class ShrinkageNuclear : public Proximal<Scalar> {
        using vec_t = Vec;
        using fmat_t = FactorizedMat<Scalar>;
        using smat_t = SpMatWrapper<Scalar>;
        vec_t d;
        Eigen::JacobiSVD<mat_t> svd; // JacobiSVD is using LAPACKE/MKL
        LinAlg::LANSVD<Scalar> lansvd;
        Scalar threshold = 1e-6_s;
        unsigned op = Eigen::DecompositionOptions::ComputeThinU |
                      Eigen::DecompositionOptions::ComputeThinV;

        Index compute_rank() const;

        public:
            inline ShrinkageNuclear(Scalar mu_ = 1) : mu(mu_) {}
            ~ShrinkageNuclear() = default;
            inline bool has_objective_cache() const { return true; }
            Scalar cached_objective() const;

            void operator()(const Ref<const mat_t>, Scalar, Ref<mat_t>);
            void operator()(const fmat_t&, Scalar, const smat_t&, Scalar, fmat_t&);
            void operator()(const var_t&, Scalar, const var_t&, Scalar, var_t&);

            Scalar mu;

    };

    template<typename dtype>
    class FuncGrad : public Functional {
        public:
            using mat_t = Eigen::Matrix<dtype, Dynamic, Dynamic>;
            using mat_wrapper_t = MatWrapper<dtype>;
            using mat_array_t = MatArray_t<dtype>;
            using var_t = Variable<dtype>;
            FuncGrad() = default;
            ~FuncGrad() = default;

            virtual Scalar operator()(const Ref<const mat_t>);
            virtual Scalar operator()(const Ref<const mat_t>, Ref<mat_t>, bool = true);
            virtual Scalar operator()(const mat_array_t&);
            virtual Scalar operator()(const mat_array_t&, mat_array_t&, bool = true);
            virtual Scalar operator()(const mat_wrapper_t&);
            virtual Scalar operator()(const mat_wrapper_t&, mat_wrapper_t&, bool = true);
            virtual Scalar operator()(const var_t&);
            virtual Scalar operator()(const var_t&, var_t&, bool = true);

    };

    template<typename dtype = Scalar>
    class AxmbNormSqr : public FuncGrad<dtype> {
        using typename FuncGrad<dtype>::mat_t;
        public:
            AxmbNormSqr(const Ref<const mat_t>, const Ref<const mat_t>);
            ~AxmbNormSqr() = default;

            Scalar       operator()(const Ref<const mat_t>, Ref<mat_t>, bool = true);
            const mat_t &get_A() const;
            const mat_t &get_b() const;

        private:
            mat_t A;
            mat_t b;
            mat_t r;
    };

    template<typename dtype = Scalar>
    class LogisticRegression : public FuncGrad<dtype> {
        using typename FuncGrad<dtype>::mat_wrapper_t;
        using typename FuncGrad<dtype>::mat_t;
        using col_vec_t = Eigen::Matrix<dtype, Dynamic, 1>;

    public:
        LogisticRegression(Ref<const mat_t>, Ref<const mat_t>);
        ~LogisticRegression() = default;

        Scalar operator()(Ref<const mat_t>, Ref<mat_t>, bool = true);

        const mat_t &    get_A() const { return A_; }
        const col_vec_t &get_b() const { return b_; }

    private:
        mat_t     A_;
        col_vec_t b_;
        mat_t     mbA_;
    };

    template<typename dtype = Scalar>
    class ProjectionOmega : public FuncGrad<dtype> {
        using typename FuncGrad<dtype>::mat_t;
        using spmat_t = Eigen::SparseMatrix<dtype, ColMajor, SparseIndex>;
        using var_t   = Variable<dtype>;
        using fmat_t  = FactorizedMat<dtype>;

    public:
        // construction by referencing a sparse object
        ProjectionOmega(const Ref<const spmat_t>, const Ref<const mat_t>);

        ~ProjectionOmega() = default;

        Scalar operator()(const Ref<const mat_t>, Ref<mat_t>, bool = true);
        Scalar operator()(const var_t &, var_t &, bool = true);

    private:
        void                     projection(const Ref<const mat_t>);
        void                     projection(const fmat_t &);
        std::vector<SparseIndex> outerIndexPtr;
        std::vector<SparseIndex> innerIndexPtr;
        mat_t                    b;
        mat_t                    r;
    };

    }   // namespace Base
    }   // namespace OptSuite

#endif
