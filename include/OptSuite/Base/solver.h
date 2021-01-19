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

enum class StepSizeStrategy
{
    Fixed,
    Deminishing,
    Deminishing2,
    Armijo,
    BBStepSize,
};

class FixedStepSize {
public:
    FixedStepSize() = default;
    FixedStepSize(Scalar t0) : t0_(t0) {}
    Scalar operator()() const { return t0_; }

private:
    Scalar t0_;
};
class Deminishing2StepSize {
public:
    Deminishing2StepSize() = default;
    Deminishing2StepSize(Index thre, Scalar t0) : thre_(thre), t0_(t0) {}
    Scalar operator()(Index iter) const {
        return t0_ / std::sqrt(std::max(thre_, iter) - thre_ + 1);
    }

private:
    Index  thre_;
    Scalar t0_;
};

class ArmijoStepSize {
public:
    ArmijoStepSize() = default;
    ArmijoStepSize(Scalar t0, Scalar shrink_scale, Scalar max_line_search_iters)
        : t0_(t0), shrink_scale_(shrink_scale), max_line_search_iters_(max_line_search_iters) {}
    Scalar operator()(Ref<const Mat> x, const MatWrapper<Scalar> &grad_f, Scalar f_val,
                      FuncGrad<Scalar> &func_f, Proximal<Scalar> &h_prox) const;

private:
    Scalar t0_;
    Scalar shrink_scale_;
    Index  max_line_search_iters_;
};

class BBStepSize {
public:
    BBStepSize() = default;
    BBStepSize(Scalar alpha, Scalar alpha_min, Scalar alpha_max, 
               Scalar shrink_scale, Scalar sigma, Scalar eta,
               Scalar max_line_search_iters, bool steptype = true)
        : alpha_(alpha), alpha0_(alpha), alpha_min_(alpha_min), alpha_max_(alpha_max), shrink_scale_(shrink_scale),
          sigma_(sigma), eta_(eta), max_line_search_iters_(max_line_search_iters), stepType(steptype) {}
    Scalar operator()(Ref<const Mat> x, const MatWrapper<Scalar> &grad_f, Scalar f_val,
                      FuncGrad<Scalar> &func_f, Func<Scalar> &func_h, Proximal<Scalar> &h_prox);
private:
    Scalar alpha_;
    Scalar alpha0_;
    Scalar alpha_min_;
    Scalar alpha_max_;
    Scalar shrink_scale_;
    Scalar sigma_;
    Scalar eta_;                                                   
    Index  max_line_search_iters_;
    Scalar C = std::numeric_limits<Scalar>::infinity();
    Scalar Q = 1;
    bool   stepType;
};

class SolverOptions {

public:
    SolverOptions() = default;
    // getter
    Scalar                      ftol();
    Scalar                      gtol();
    Scalar                      xtol();
    Index                       maxit();
    Index                       min_lasting_iters() const { return min_lasting_iters_; }
    const ArmijoStepSize &      armijo() const { return armijo_; }
    const Deminishing2StepSize &deminishing2() const { return deminishing2_; }
    const FixedStepSize &       fixed() const { return fixed_; }
    const StepSizeStrategy &    step_size_strategy() { return step_size_strategy_; }
    BBStepSize &                bb() {return bb_;}
    Verbosity                   verbosity();

    // setter
    void ftol(Scalar);
    void gtol(Scalar);
    void xtol(Scalar);
    void maxit(Index);
    void min_lasting_iters(Index min_lasting_iters) { min_lasting_iters_ = min_lasting_iters; }
    void verbosity(Verbosity);
    void step_size_strategy(StepSizeStrategy step_size_strategy) {
        step_size_strategy_ = step_size_strategy;
    }
    void armijo(const ArmijoStepSize &armijo) { armijo_ = armijo; }
    void deminishing2(const Deminishing2StepSize &deminishing2) { deminishing2_ = deminishing2; }
    void fixed(const FixedStepSize &fixed) { fixed_ = fixed; }
    void bb(BBStepSize &bb) { bb_ = bb; }

protected:
    Scalar ftol_;   ///< The objective value variation tolerance
    Scalar gtol_;   ///< The gradient norm tolerance
    Scalar xtol_;   ///< The independent variable tolerance

    Index                maxit_;
    Verbosity            verbosity_;
    Index                min_lasting_iters_;
    StepSizeStrategy     step_size_strategy_;
    ArmijoStepSize       armijo_;
    Deminishing2StepSize deminishing2_;
    FixedStepSize        fixed_;
    BBStepSize           bb_;
};

struct SolverRecords {
    Index               n_iters;
    std::vector<Scalar> obj_hist;
    time_t              elapsed_time_us;
};

class ProximalGradSolver : public SolverBase {
public:
    ProximalGradSolver(std::string name, SolverOptions options)
        : SolverBase(std::move(name)), options_(options) {}
    void operator()(Ref<const Mat> x0, FuncGrad<Scalar> &func_f, Func<Scalar> &func_h,
                    Proximal<Scalar> &h_prox, Scalar t, Ref<Mat> result, SolverRecords &records);

protected:
    SolverOptions options_;
};
}   // namespace Base
}   // namespace OptSuite


#endif
