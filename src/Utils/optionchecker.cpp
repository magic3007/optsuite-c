/*
 * ===========================================================================
 *
 *       Filename:  optionchecker.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/21/2020 03:33:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, Peking University
 *      Copyright:  Copyright (c) 2020, Haoyang Liu
 *
 * ===========================================================================
 */

#include "OptSuite/core_n.h"
#include "OptSuite/Utils/optionchecker.h"

namespace OptSuite { namespace Utils {
    template<typename T>
    BoundChecker<T>::BoundChecker(T lower, BoundCheckerSense lower_s,
                               T upper, BoundCheckerSense upper_s){
        set_lower(lower, lower_s);
        set_upper(upper, upper_s);
    }

    template<typename T>
    void BoundChecker<T>::set_lower(T lower, BoundCheckerSense lower_s){
        this->lower = lower;
        this->lower_sense = lower_s;
    }

    template<typename T>
    void BoundChecker<T>::set_upper(T upper, BoundCheckerSense upper_s){
        this->upper = upper;
        this->upper_sense = upper_s;
    }

    template<typename T>
    bool BoundChecker<T>::operator()(const T& v){
        // check lower bound
        switch (lower_sense) {
            case BoundCheckerSense::Standard:
                if (lower > v) return false;
                break;
            case BoundCheckerSense::Strict:
                if (lower >= v) return false;
                break;
            default:
                break;
        }

        // check upper bound
        switch (upper_sense) {
            case BoundCheckerSense::Standard:
                if (v > upper) return false;
                break;
            case BoundCheckerSense::Strict:
                if (v >= upper) return false;
                break;
            default:
                break;
        }
        return true;
    }

    // instantiate
    template class BoundChecker<Index>;
    template class BoundChecker<Scalar>;
}}
