/*
 * ===========================================================================
 *
 *       Filename:  block.cpp
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  11/13/2020 09:42:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, Peking University
 *      Copyright:  Copyright (c) 2020, Haoyang Liu
 *
 * ===========================================================================
 */

#include <iostream>
#include "OptSuite/core_n.h"
#include "OptSuite/Base/mat_array.h"
#include "OptSuite/Base/mat_wrapper.h"
#include "OptSuite/Base/spmat_wrapper.h"
#include "OptSuite/LinAlg/rng_wrapper.h"

using namespace OptSuite;
using namespace OptSuite::Base;
using namespace OptSuite::LinAlg;

int main(int argc, char **argv){
    MatArray bv;

    bv.add_blocks(3, 1, 3);
    bv.add_blocks(2, 2, 2);
    
    std::cout << bv.total_size() << " " << bv.total_blocks() << std::endl;

    bv.vec() = randn(bv.total_size(), 1);

    std::cout << bv << std::endl;

    MatArray bv2;
    bv2.set_zero_like(bv);
    std::cout << bv2 << std::endl;

    MatArray bv3 = MatArray::from_vec_like(bv.vec() * 2, bv);
    std::cout << bv3 << std::endl;

    std::cout << bv.dot(bv3) << std::endl;

    MatWrapper<Scalar> mw(bv[1]);
    std::cout << mw.mat() << std::endl;
    mw.set_zero_like(bv[3]);
    std::cout << mw.mat() << std::endl;

    SpMatWrapper<Scalar> spw(sprandn(5, 5, 0.25));
    std::cout << spw.spmat() << std::endl;

    spw.set_zero_like(sprandn(5, 5, 0.25));
    std::cout << spw.spmat().nonZeros() << " " << spw.spmat() << std::endl;


    return 0;
}
