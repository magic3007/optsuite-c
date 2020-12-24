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

#include "OptSuite/core_n.h"
#include "OptSuite/Base/solver.h"

namespace OptSuite { namespace Base {
    Scalar SolverOptions::ftol() { return ftol_; }
    Scalar SolverOptions::gtol() { return gtol_; }
    Scalar SolverOptions::xtol() { return xtol_; }

    Index SolverOptions::maxit() { return maxit_; }
    Verbosity SolverOptions::verbosity() { return verbosity_; }

    void SolverOptions::ftol(Scalar v){
        OPTSUITE_ASSERT(v >= 0);
        ftol_ = v;
    }
    void SolverOptions::gtol(Scalar v){
        OPTSUITE_ASSERT(v >= 0);
        gtol_ = v;
    }
    void SolverOptions::xtol(Scalar v){
        OPTSUITE_ASSERT(v >= 0);
        xtol_ = v;
    }

    void SolverOptions::maxit(Index v){
        OPTSUITE_ASSERT(v > 0);
        maxit_ = v;
    }
    void SolverOptions::verbosity(Verbosity v){
        verbosity_ = v;
    }
}}

