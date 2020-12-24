/*
 * ==========================================================================
 *
 *       Filename:  rng_wrapper.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/06/2020 11:19:07 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#ifndef OPTSUITE_LINALG_RNG_WRAPPER_H
#define OPTSUITE_LINALG_RNG_WRAPPER_H

#include <set>
#include <random>
#include "OptSuite/core_n.h"


namespace OptSuite { namespace LinAlg {

    Mat randn(Size, Size, Scalar = 0, Scalar = 1);
    Mat rand(Size, Size, Scalar = 0, Scalar = 1);
    SpMat sprandn(Size, Size, Scalar);
    SpMat sprandn_c(Size, Size, Scalar);

    const std::mt19937& get_generator();
    void rng(unsigned long);
}}

#endif
