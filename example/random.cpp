/*
 * ==========================================================================
 *
 *       Filename:  random.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/06/2020 11:59:48 AM
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
#include "OptSuite/rng_wrapper.h"

using namespace OptSuite;

int main(int argc, char **argv){
    SpMat sm = sprandn(5, 5, 0.2);
    return 0;
}
