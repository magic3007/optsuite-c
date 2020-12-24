/*
 * ===========================================================================
 *
 *       Filename:  optionchecker.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/21/2020 03:20:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, Peking University
 *
 * ===========================================================================
 */

#ifndef OPTSUITE_UTILS_OPTIONCHECKER_H
#define OPTSUITE_UTILS_OPTIONCHECKER_H

#include <set>
#include <initializer_list>
#include "OptSuite/core_n.h"

namespace OptSuite { namespace Utils {
    enum class BoundCheckerSense {
        None,
        Standard,
        Strict
    };

    class OptionChecker {
        public:
            inline virtual bool operator()(const std::string&){
                return true;
            }
            inline virtual bool operator()(const Index&){
                return true;
            }
            inline virtual bool operator()(const Scalar&){
                return true;
            }
            inline virtual bool operator()(const bool&){
                return true;
            }
    };

    template<typename T>
    class BoundChecker : public OptionChecker {
        T lower;
        T upper;
        BoundCheckerSense lower_sense;
        BoundCheckerSense upper_sense;

        public:
            BoundChecker(T, BoundCheckerSense, T, BoundCheckerSense);
            void set_lower(T, BoundCheckerSense);
            void set_upper(T, BoundCheckerSense);
            bool operator()(const T&);
    };

    class StrEnumChecker : public OptionChecker {
        std::set<std::string> collection;

        public:
            StrEnumChecker() = default;
            inline
            StrEnumChecker(const std::set<std::string>& v) : collection(v) {}

            inline
            StrEnumChecker(std::initializer_list<std::string> l) : collection(l) {}

            inline bool operator()(const std::string& s){
                return collection.find(s) != collection.cend();
            }

            inline void insert(const std::string& s){
                collection.insert(s);
            }
    };
}}

#endif
