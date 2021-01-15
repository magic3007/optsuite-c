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
#include "OptSuite/Utils/stopwatch.hpp"
#include "OptSuite/core_n.h"

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

void ProximalGradSolver::operator()(Ref<const Mat> x0, FuncGrad<Scalar> &func_f,
                                    Func<Scalar> func_h, Proximal<Scalar> &h_prox, Scalar t,
                                    Ref<Mat> result, SolverRecords &records) {
    stopwatch::Stopwatch stopwatch;
    stopwatch.start();
    Mat                 x = x0;
    Mat                 grad_f;
    Index               i;
    std::vector<Scalar> f_hist;
    for (i = 0; i < options_.maxit(); i++) {
        Scalar f_val   = func_f(x, grad_f, true);
        Scalar h_val   = func_h(x);
        Scalar obj_val = f_val + t * h_val;
        f_hist.push_back(obj_val);
        if (options_.stop_condition_checker()(f_hist)) { break; }
        // TODO(Jing Mai, jingmai@pku.edu.cn): add step size strategies.
        Scalar step_size = 1;
        h_prox(x.array() - step_size * grad_f.array(), /* t */ t, x);
    }
    result                  = x;
    records.elapsed_time_us = stopwatch.elapsed<stopwatch::mus>();
    records.n_iters         = i;
    records.f_hist          = std::move(f_hist);
}
}   // namespace Base
}   // namespace OptSuite
