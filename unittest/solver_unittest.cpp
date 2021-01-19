/**
 * File              : solver_unittest.cpp
 * Author            : Jiarui Wang <jiaruiwang@pku.edu.cn>
 * Date              : 01.18.2021
 */

#include "OptSuite/Base/mat_wrapper.h"
#include "OptSuite/Base/solver.h"
#include "OptSuite/LinAlg/rng_wrapper.h"
#include "OptSuite/Utils/logger.h"
#include "OptSuite/core_n.h"
#include <algorithm>
#include <iomanip>
#include <random>
#include <vector>


using namespace OptSuite;
using namespace OptSuite::Utils;
using namespace OptSuite::Base;
using namespace OptSuite::LinAlg;

enum class F_Function_Type {
    AxmbNormSqr,
    LogisticRegression
};

enum class H_Function_Type {
    IdentityProx,
    L1NormBallProj,
    L0NormBallProj,
    L2NormBallProj,
    LInfBallProj,
    ShrinkageL1,
    ShrinkageL2,
    ShrinkageL0,
    ShrinkageL2Rowwise,
    ShrinkageNuclear
};

enum class Optimizer_Type {
    Fixed,
    Armijo,
    BBStepSize1,
    BBStepSize2
};

int main(int argc, char **argv) {


    Index                    m = 256, n = 512;
    Base::MatWrapper<Scalar> A, u, b;
    Scalar                   mu = 1e-3;

    rng(/* seed */ 114514);

    A.mat() = randn(m, n);
    u.mat() = randn(n, 1);
    /* Sparsify */ {
        std::vector<Index> indexes(n);
        std::iota(indexes.begin(), indexes.end(), 0);
        std::shuffle(indexes.begin(), indexes.end(), std::mt19937(std::random_device()()));
        Index k = std::floor(n * 0.1);
        for (Index i = k; i < n; i++) { u.mat().coeffRef(i) = 0; }
    }
    b.mat()        = A.mat() * u.mat();
    auto err_exact = [&](const Base::MatWrapper<Scalar> &x) {
        return u.squared_norm_diff(x) / (1 + u.mat().squaredNorm());
    };
    auto sparsity = [&](const Base::MatWrapper<Scalar> &x) {
        Scalar thre = 1e-6 * x.mat().array().abs().maxCoeff();
        return 1.0 * (x.mat().array().abs() > thre).count() / n;
    };
    /* 0.5*|Ax-b|^2_2 + mu * |x|_1 */ {
        Base::MatWrapper<Scalar> x0;
        x0.mat() = randn(n, 1);

        auto                     func_f = Base::AxmbNormSqr<Scalar>(A.mat(), b.mat());
        auto                     func_h = Base::L1Norm(mu);
        auto                     h_prox = Base::ShrinkageL1(mu);
        Base::SolverRecords      records;
        Base::MatWrapper<Scalar> result(x0);
        Base::MatWrapper<Scalar> grad_f(x0);

        func_f(x0.mat(), grad_f.mat(), true);
        Scalar              t0 = x0.mat().norm() / grad_f.mat().norm() * 6e-1;
        Base::SolverOptions options{};
        options.ftol(1e-5);
        options.maxit(10000);
        options.min_lasting_iters(100);
        options.step_size_strategy(Base::StepSizeStrategy::BBStepSize);
        options.fixed(Base::FixedStepSize(t0));
        options.verbosity(Verbosity::Debug);
        Base::ArmijoStepSize armijo(t0, 0.98, 10);
        options.armijo(armijo);
        Base::BBStepSize bb(t0, 1e-20, 1e20, 0.98, 1e-4, 0.85, 5);
        options.bb(bb);
        Base::ProximalGradSolver solver("Proximal Gradient", options);


        for (Scalar t : {1}) {
            Utils::Global::logger_o.log_info("=======================\n");
            solver(x0.mat(), func_f, func_h, h_prox, t, result.mat(), records);
            x0.mat() = result.mat();
        }
        Utils::Global::logger_o.log_info(std::left, std::setw(10), "Iters: ", std::left,
                                         records.n_iters);
        Utils::Global::logger_o.log_info(std::left, std::setw(10), ", Elapsed time: ", std::left,
                                         std::setprecision(6), records.elapsed_time_us / 1e6);
        Utils::Global::logger_e.log_info(std::left, std::setw(10), ", Err-exact: ", std::left,
                                         std::scientific, err_exact(result));
        Utils::Global::logger_e.log_info(std::left, std::setw(10), ", Sparsity: ", std::fixed,
                                         std::setprecision(5), sparsity(result));
    }
    return 0;
}