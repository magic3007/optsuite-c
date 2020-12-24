/*
 * ==========================================================================
 *
 *       Filename:  logger.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/31/2020 05:07:29 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#ifndef OPTSUITE_UTILS_LOGGER_H
#define OPTSUITE_UTILS_LOGGER_H

#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <type_traits>

#include "OptSuite/core_n.h"
#include "OptSuite/Utils/str_format.h"

namespace OptSuite { namespace Utils {
    using std::unique_ptr;
    class Logger {
        public:
            Logger(Verbosity = Verbosity::Info, bool = false);
            Logger(unique_ptr<std::ostream>&&, Verbosity = Verbosity::Info);
            Logger(const std::string&, Verbosity = Verbosity::Info);

            Logger(const Logger &) = delete;
            Logger& operator=(const Logger&) = delete;

            Logger(Logger &&) = default;
            Logger& operator=(Logger &&) = default;

            ~Logger() = default;

            // obtain the ostream object of the logger
            std::ostream& get_stream() const;

            // redirection
            void redirect_to_file(const std::string&);
            void redirect_to_stdout();
            void redirect_to_stderr();
            
            // verbosity level
            Verbosity verbosity;

            // general log
            template <Verbosity loglevel = Verbosity::Info>
            inline
            void log() {}

            template <Verbosity loglevel = Verbosity::Info, typename T, typename... Rest>
            inline
            void log(const T& obj, const Rest&... rest) {
                auto& out = get_stream();
                if (verbosity >= loglevel) {
                    out << obj;
                    log<loglevel>(rest...);
                }
            }

            // instantiate
            template <typename... Args>
            inline
            void log_info(const Args&... args){
                log<Verbosity::Info>(args...);
            }

            template <typename... Args>
            inline
            void log_debug(const Args&... args){
                log<Verbosity::Debug>(args...);
            }

            template <Verbosity loglevel = Verbosity::Info, typename... Args>
            inline
            void log_format(const char* format, Args&&... args) {
                log<loglevel>(cstr_format(format, std::forward<Args>(args)...).get());
            }
            
        private:
            unique_ptr<std::ostream> out_ptr = nullptr;
            bool use_stdout = false;
            bool use_stderr = false;
    };

    namespace Global {
        extern Logger logger_e;
        extern Logger logger_o;
    }

}}

#endif
