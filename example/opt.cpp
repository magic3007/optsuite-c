/*
 * ==========================================================================
 *
 *       Filename:  opt.cpp
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  11/20/2020 03:46:26 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#include <memory>
#include <vector>
#include "OptSuite/core_n.h"
#include "OptSuite/Utils/logger.h"
#include "OptSuite/Utils/optionlist.h"
#include "OptSuite/Utils/optionchecker.h"

using namespace OptSuite;
using namespace OptSuite::Utils;

int main(int argc, char **argv){
    std::vector<RegOption> v;
    std::shared_ptr<OptionChecker> checker(new StrEnumChecker
            {"A", "A-", "B+", "B", "B-"});

    v.push_back({"name", "", "input your name"});
    v.push_back({"gpa", 4.0, "current gpa, must in [0, 4]",
            std::make_shared<BoundChecker<Scalar>>(0, BoundCheckerSense::Standard, 4, BoundCheckerSense::Standard)});
    v.push_back({"age", 9_i, "Age. Must be positive",
            std::make_shared<BoundChecker<Index>>(0, BoundCheckerSense::Strict, 0, BoundCheckerSense::None)});
    v.push_back({"grade", "A", "grade, must be one of A, A-, B+, B, B-", checker});

    OptionList ol(v, std::make_shared<Logger>());

    // register on-the-fly
    ol.register_option({"school", "school of mathematical sciences", "school game"});

    // options from cmd line have higher priority
    ol.set_from_file("opt.conf");
    ol.set_from_cmd_line(argc, argv);

    // print out
    ol.show(true);

    return 0;
}
