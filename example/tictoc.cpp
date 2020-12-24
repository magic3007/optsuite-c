/*
 * ==========================================================================
 *
 *       Filename:  tictoc.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/31/2020 06:10:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */
#include <iostream>
#include <thread>
#include <chrono>
#include "OptSuite/tictoc.h"

using namespace OptSuite;

int main(int argc, char **argv){
    auto tstart = tic();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << toc(tstart) << std::endl;

    return 0;
}
