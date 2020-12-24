/*
 * ==========================================================================
 *
 *       Filename:  readmat.cpp
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  11/28/2020 01:43:11 PM
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
#include "OptSuite/Interface/matlab.h"

using namespace OptSuite;
using namespace OptSuite::Interface;

int main(int argc, char **argv){
    Mat A;
    SpMat spA;
    get_matrix_from_file("../data/X.mat", "X", A);
    get_matrix_from_file("../data/X.mat", "spX", spA);

    std::cout << A << std::endl;
    std::cout << spA << std::endl;
}
