/*
 * ==========================================================================
 *
 *       Filename:  solver.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/05/2020 03:39:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#ifndef OPTSUITE_BASE_SOLVER_H
#define OPTSUITE_BASE_SOLVER_H

#include "OptSuite/Base/functional.h"
#include "OptSuite/core_n.h"
#include <functional>
#include <string>

namespace OptSuite {
namespace Base {
class SolverBase {
public:
    SolverBase(std::string name_) : name(name_) {}
    ~SolverBase() = default;

    inline virtual void read_options_from_file(std::string) {}

    std::string name;
};

class SolverOptions {
    using StopConditionChecker = typename std::function<bool(const std::vector<Scalar> &)>;

public:
    // getter
    Scalar                      ftol();
    Scalar                      gtol();
    Scalar                      xtol();
    Index                       maxit();
    Verbosity                   verbosity();
    const StopConditionChecker &stop_condition_checker() { return checker_; }
    // setter
    void ftol(Scalar);
    void gtol(Scalar);
    void xtol(Scalar);
    void maxit(Index);
    void verbosity(Verbosity);
    void stop_condition_checker(StopConditionChecker checker) { checker_ = std::move(checker); }

protected:
    Scalar ftol_;   ///< The objective value variation tolerance
    Scalar gtol_;   ///< The gradient norm tolerance
    Scalar xtol_;   ///< The independent variable tolerance

    Index                maxit_;
    Verbosity            verbosity_;
    StopConditionChecker checker_;
};

struct SolverRecords {
    Index               n_iters;
    std::vector<Scalar> f_hist;
    time_t              elapsed_time_us;
};

class ProximalGradSolver : public SolverBase {
public:
    ProximalGradSolver(std::string name, SolverOptions options)
        : SolverBase(name), options_(options) {}
    void operator()(Ref<const Mat> x0, FuncGrad<Scalar> &func_f, Func<Scalar> func_h,
                    Proximal<Scalar> &h_prox, Scalar t, Ref<Mat> result, SolverRecords &records);

protected:
    SolverOptions options_;
};
}   // namespace Base
}   // namespace OptSuite


#endif
