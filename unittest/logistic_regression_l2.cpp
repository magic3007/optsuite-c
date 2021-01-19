/**
 * File              : lasso.cpp
 * Author            : Jing Mai <jingmai@pku.edu.cn>
 * Date              : 01.16.2021
 * Last Modified Date: 01.16.2021
 * Last Modified By  : Jing Mai <jingmai@pku.edu.cn>
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

int main(int argc, char **argv) {


    Index                    m = 2560, n = 512;
    Base::MatWrapper<Scalar> A, u, b;
    Scalar                   mu = 1e-3;

    rng(/* seed */ 114514);

    A.mat() = randn(n, m);
    u.mat() = randn(n, 1);
    b.mat() = randn(m, 1);
    for (Index i = 0; i < m; i++)
        b.mat().coeffRef(i) = (rand() % 2 == 0 ? 1 : -1);
    auto err_exact = [&](const Base::MatWrapper<Scalar> &x) {
        Scalar correct = 0;
        for (Index i = 0; i < m; i++)
            if (x.mat().array().coeffRef(i) * b.mat().coeffRef(i) > 0) correct++;
        return correct / m;
    };
    auto sparsity = [&](const Base::MatWrapper<Scalar> &x) {
        Scalar thre = 1e-6 * x.mat().array().abs().maxCoeff();
        return 1.0 * (x.mat().array().abs() > thre).count() / n;
    };
    {
        Base::MatWrapper<Scalar> x0;
        x0.set_zero_like(u);

        auto                     func_f = Base::LogisticRegression<Scalar>(A.mat(), b.mat());
        auto                     func_h = Base::L2Norm(mu);
        Base::SolverRecords      records;
        Base::MatWrapper<Scalar> result(x0);
        Base::MatWrapper<Scalar> grad_f(x0);

        func_f(x0.mat(), grad_f.mat(), true);
        // Scalar              t0 = x0.mat().norm() / grad_f.mat().norm() * 6e-1;
        Scalar t0 = 0.001;
        Base::SolverOptions options{};
        options.ftol(1e-5);
        options.maxit(1000000);
        options.min_lasting_iters(100);
        options.step_size_strategy(Base::StepSizeStrategy::Armijo);
        options.fixed(Base::FixedStepSize(t0));
        options.verbosity(Verbosity::Debug);
        Base::ArmijoStepSize armijo(t0, 0.6, 5);
        options.armijo(armijo);
        Base::BBStepSize bb(t0, 1e-20, 1e20, 0.6, 1e-4, 0.85, 5, true);
        options.bb(bb);
        Base::Deminishing2StepSize d2(1e-5, t0);
        options.deminishing2(d2);
        Base::ProximalGradSolver solver("Proximal Gradient", options);


        for (Scalar t : {1}) {
            auto                     h_prox = Base::ShrinkageL2(t * mu);
            Utils::Global::logger_o.log_info("=======================\n");
            solver(x0.mat(), func_f, func_h, h_prox, t, result.mat(), records);
            x0.mat() = result.mat();
        }

        MatWrapper<Scalar> b_pred(A.mat().transpose() * result.mat());
        Utils::Global::logger_o.log_info(std::left, std::setw(10), "Iters: ", std::left,
                                         records.n_iters);
        Utils::Global::logger_o.log_info(std::left, std::setw(10), ", Elapsed time: ", std::left,
                                         std::setprecision(6), records.elapsed_time_us / 1e6);
        Utils::Global::logger_e.log_info(std::left, std::setw(10), ", Precision: ", std::left,
                                         std::scientific, err_exact(b_pred));
        Utils::Global::logger_e.log_info(std::left, std::setw(10), ", Sparsity: ", std::fixed,
                                         std::setprecision(5), sparsity(result));
    }
    return 0;
}