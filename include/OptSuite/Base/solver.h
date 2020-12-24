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

#include <string>
#include "OptSuite/core_n.h"

namespace OptSuite { namespace Base {
    class SolverBase {
        public:
            SolverBase(std::string name_) : name(name_) {}
            ~SolverBase() = default;

            inline
            virtual void read_options_from_file(std::string) {}

            std::string name;
    };

    class SolverOptions {
        public:
            // getter
            Scalar ftol();
            Scalar gtol();
            Scalar xtol();
            Index maxit();
            Verbosity verbosity();

            // setter
            void ftol(Scalar);
            void gtol(Scalar);
            void xtol(Scalar);
            void maxit(Index);
            void verbosity(Verbosity);

        protected:
            Scalar ftol_;
            Scalar gtol_;
            Scalar xtol_;

            Index maxit_;
            Verbosity verbosity_;
    };
}}

#endif
