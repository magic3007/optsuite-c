/*
 * ==========================================================================
 *
 *       Filename:  variable.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/10/2020 02:32:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#ifndef OPTSUITE_BASE_VARIABLE_H
#define OPTSUITE_BASE_VARIABLE_H
#include "OptSuite/core_n.h"

namespace OptSuite { namespace Base {
    template<typename T>
    class Variable {
        public:
            Variable() = default;
            ~Variable() = default;

            // operations
            virtual Variable<T>& operator=(const Variable<T>&) { return *this; } ;
            virtual T dot(const Variable<T>&) const = 0;
            virtual void set_zero_like(const Variable<T>&) = 0;

            inline
            virtual Scalar squared_norm_diff(const Variable<T>&) const { return 0_s; }

            inline
            virtual bool has_squared_norm_diff() const { return false; }

    };
}}

#endif
