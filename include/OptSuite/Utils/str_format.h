/*
 * ==========================================================================
 *
 *       Filename:  str_format.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/22/2020 03:08:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#ifndef OPTSUITE_UTILS_STR_FORMAT_H
#define OPTSUITE_UTILS_STR_FORMAT_H

#include <memory>

namespace OptSuite { namespace Utils {

    // string formatting (C style)
    template <typename... Args>
    std::unique_ptr<char[]> cstr_format(const char* format, Args&&... args) {
        using std::snprintf;
        auto size = snprintf(nullptr, 0, format, args...) + 1; // Extra space for '\0'
        auto buf = std::unique_ptr<char[]>(new char[size]);
        snprintf(buf.get(), size, format, args...);
        return buf;
    }

    // sprintf to std::string
    // Original source: https://stackoverflow.com/a/26221725/11722
    template <typename... Args>
    std::string str_format(const char* format, Args&&... args) {
        using std::snprintf;
        auto size = snprintf(nullptr, 0, format, args...) + 1; // Extra space for '\0'
        auto buf = std::unique_ptr<char[]>(new char[size]);
        snprintf(buf.get(), size, format, args...);
        return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
    }

}}

#endif
