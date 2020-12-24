/*
 * ==========================================================================
 *
 *       Filename:  fft.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/26/2020 01:22:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#include <iostream>
#include <complex>
#include <vector>
#include "OptSuite/core_n.h"
#include "OptSuite/LinAlg/fftw_wrapper.h"

using namespace OptSuite;
using namespace OptSuite::LinAlg;


int main(int argc, char **argv) {
    CMat in(16, 2);

    in(0, 0) = 1;
    in(3, 0) = {0, 2};

    in(1, 1) = 2;
    in(4, 1) = {2, 5};

    CMat out = fft(in);
    in = ifft(out);
    
    std::cout << out << std::endl;
    std::cout << in << std::endl;

    return 0;
}
