/*
 * ==========================================================================
 *
 *       Filename:  svd.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/03/2020 09:50:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */
#include <iostream>
#include "OptSuite/core_n.h"
#include "OptSuite/Base/functional.h"
#include "OptSuite/Base/mat_op.h"
#include "OptSuite/Utils/logger.h"
#include "OptSuite/Utils/tictoc.h"
#include "OptSuite/LinAlg/rng_wrapper.h"
#include "OptSuite/LinAlg/lansvd.h"
#include "OptSuite/Interface/matlab.h"

using namespace OptSuite;
using namespace OptSuite::Base;
using namespace OptSuite::LinAlg;
using namespace OptSuite::Utils;
using namespace OptSuite::Interface;

int main(int argc, char **argv){
    Mat A;
    SpMat spA;
    get_matrix_from_file("../data/example.mat", "A", A);
    get_matrix_from_file("../data/example.mat", "spA", spA);

    // construct factorized mat and sparse
    FactorizedMat<Scalar> mf(A, A);
    SpMatWrapper<Scalar> ms(spA);

    // construct op
    FactorizePSpMatOp<Scalar> Aop(mf, 0_s, ms);

    auto t = tic();
    LANSVD<Scalar> lansvd;
    lansvd.options().set_from_cmd_line(argc, argv);
    lansvd.compute(Aop, 50);
    std::cout << toc(t) << " sec." << std::endl;

    std::cout << "Terminated with " << lansvd.info() << std::endl;
    std::cout << lansvd.d() << std::endl;
    std::cout << lansvd.bnd() << std::endl;

    return 0;
}
