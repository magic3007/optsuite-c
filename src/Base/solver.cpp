/*
 * ==========================================================================
 *
 *       Filename:  solver.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/06/2020 06:46:26 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#include "OptSuite/Base/solver.h"
#include "OptSuite/Utils/logger.h"
#include "OptSuite/Utils/stopwatch.hpp"
#include "OptSuite/core_n.h"
#include <iomanip>

namespace OptSuite {
namespace Base {
Scalar SolverOptions::ftol() { return ftol_; }
Scalar SolverOptions::gtol() { return gtol_; }
Scalar SolverOptions::xtol() { return xtol_; }

Index     SolverOptions::maxit() { return maxit_; }
Verbosity SolverOptions::verbosity() { return verbosity_; }

void SolverOptions::ftol(Scalar v) {
    OPTSUITE_ASSERT(v >= 0);
    ftol_ = v;
}
void SolverOptions::gtol(Scalar v) {
    OPTSUITE_ASSERT(v >= 0);
    gtol_ = v;
}
void SolverOptions::xtol(Scalar v) {
    OPTSUITE_ASSERT(v >= 0);
    xtol_ = v;
}

void SolverOptions::maxit(Index v) {
    OPTSUITE_ASSERT(v > 0);
    maxit_ = v;
}
void SolverOptions::verbosity(Verbosity v) { verbosity_ = v; }

Scalar ArmijoStepSize::operator()(Ref<const Mat> x, const MatWrapper<Scalar> &grad_f, Scalar f_val,
                                  FuncGrad<Scalar> &func_f, Proximal<Scalar> &h_prox) const {
    auto func_gt = [&](Scalar t) {
        MatWrapper<Scalar> temp(x);
        // h_prox(x.array() - t * grad_f.mat().array(), t, temp);
        h_prox(x - t * grad_f.mat(), t, temp.mat());
        Mat gt = (x - temp.mat()) / t;
        return gt;
    };
    Scalar t = t0_;
    for (Index i = 0; i < max_line_search_iters_; i++) {
        MatWrapper<Scalar> gt(x);
        gt  = MatWrapper<Scalar>(func_gt(t));
        Scalar             lhs = func_f(x - t * gt.mat()); 
        Scalar             rhs = f_val + t * grad_f.dot(gt) + 0.5 * t * gt.mat().squaredNorm();
        if (lhs <= rhs) { break; }
        t *= shrink_scale_;
    }
    return t;
}

Scalar BBStepSize::operator()(Ref<const Mat> x, const MatWrapper<Scalar> &grad_f, Scalar f_val,
                                FuncGrad<Scalar> &func_f, Func<Scalar> &func_h, Proximal<Scalar> &h_prox) {
    auto func_xbar = [&](Scalar t) {
        MatWrapper<Scalar> temp(x);
        h_prox(x - t * grad_f.mat(), t, temp.mat());
        return temp;
    };
    if (C == std::numeric_limits<Scalar>::infinity())
        C = f_val;
    Mat x_new(x);
    for (Index i = 0; i < max_line_search_iters_; i++) {
        MatWrapper<Scalar> xbar(x);
                           xbar = MatWrapper<Scalar>(func_xbar(alpha_));
        MatWrapper<Scalar>    d(xbar.mat() - x);
        Scalar            delta = grad_f.dot(d) + func_h(xbar) - func_h(x);
                          x_new = x + alpha_ * d.mat();
        Scalar             lhs  = func_f(x_new) + func_h(x_new);
        Scalar             rhs  = C + sigma_ * alpha_ * delta; 
        if (lhs < rhs) break;
        alpha_ *= shrink_scale_;
    }
    Scalar ret = alpha_;
    MatWrapper<Scalar> grad_f_new(x_new);
    Scalar f_val_new = func_f(x_new, grad_f_new.mat(), true);
    MatWrapper<Scalar> s(x_new - x);
    MatWrapper<Scalar> y(grad_f_new.mat() - grad_f.mat());
    C = (eta_ * Q * C + f_val_new) / (eta_ * Q + 1);
    Q = eta_ * Q + 1;
    if (stepType)
        alpha_ = (s.dot(y) == 0 ? alpha0_ : s.dot(s) / s.dot(y));
    else 
        alpha_ = s.dot(y) / y.dot(y);
    // stepType ^= 1;
    alpha_ = std::min(alpha_max_, std::max(alpha_min_, alpha_));
    return ret;
}

void ProximalGradSolver::operator()(Ref<const Mat> x0, FuncGrad<Scalar> &func_f,
                                    Func<Scalar> &func_h, Proximal<Scalar> &h_prox, Scalar t,
                                    Ref<Mat> result, SolverRecords &records) {
    using namespace OptSuite::Utils;
    Logger               logger(options_.verbosity(), /* use_stderr */ true);
    stopwatch::Stopwatch stopwatch;
    stopwatch.start();
    Mat                 x = x0;
    MatWrapper<Scalar>  grad_f(x);
    Index               i;
    std::vector<Scalar> obj_hist;
    Index               lasting_iters = 0;
    Scalar              f_val, h_val;
    auto                stop_checker = [&]() -> bool {
        Index size = obj_hist.size();
        if (size < 2) { return false; }
        if (std::fabs(obj_hist[size - 1] - obj_hist[size - 2]) / obj_hist[size - 2] <
            options_.ftol()) {
            lasting_iters++;
        } else {
            lasting_iters = 0;
        }
        return lasting_iters >= options_.min_lasting_iters();
    };
    auto get_step_size = [&]() -> Scalar {
        switch (this->options_.step_size_strategy()) {
            case StepSizeStrategy::Fixed:
                return this->options_.fixed()();
            case StepSizeStrategy::Deminishing2:
                return this->options_.deminishing2()(i);
            case StepSizeStrategy::Armijo:
                return this->options_.armijo()(x, grad_f, f_val, func_f, h_prox);
            case StepSizeStrategy::BBStepSize:
                return this->options_.bb()(x, grad_f, f_val, func_f, func_h, h_prox);
            default:
                OPTSUITE_ASSERT(false);
        }
    };
    for (i = 0; i < options_.maxit(); i++) {
        f_val          = func_f(x, grad_f.mat(), true);
        h_val          = func_h(x);
        Scalar obj_val = f_val + t * h_val;
        if (i % 10 == 0) {
            logger.log_debug(std::left, std::setw(10), "Iters: ", i);
            logger.log_debug(std::left, std::scientific, ", Obj: ", obj_val);
            logger.log_debug(std::left, std::scientific, ", f_val: ", f_val);
            logger.log_debug("\n");
        }
        obj_hist.push_back(obj_val);
        if (stop_checker()) { break; }
        Scalar step_size = get_step_size();
        h_prox(x.array() - step_size * grad_f.mat().array(), /* t */ step_size, x);
    }
    result                  = x;
    records.elapsed_time_us += stopwatch.elapsed<stopwatch::mus>();
    records.n_iters         += i;
    records.obj_hist        = std::move(obj_hist);
}
}   // namespace Base
}   // namespace OptSuite
