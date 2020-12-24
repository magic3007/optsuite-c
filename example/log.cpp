/*
 * ==========================================================================
 *
 *       Filename:  log.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/31/2020 07:27:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#include <iostream>
#include <fstream>
#include "OptSuite/logger.h"

using namespace OptSuite;

int main(int argc, char **argv){
    Logger logger;

    logger.log_format("taroxd cuxia! %05d\n", 666);

    return 0;
}
