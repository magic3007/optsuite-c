/*
 * ==========================================================================
 *
 *       Filename:  logger.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/31/2020 06:48:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#include "OptSuite/Utils/logger.h"

namespace OptSuite { namespace Utils {
    namespace Global {
        Logger logger_e(Verbosity::Info, true);
        Logger logger_o{};
    }

    Logger::Logger(Verbosity v, bool use_stderr){
        verbosity = v;
        if (use_stderr)
            this->use_stderr = true;
        else
            this->use_stdout = true;
    }

    Logger::Logger(unique_ptr<std::ostream>&& o, Verbosity v){
        verbosity = v;
        out_ptr = std::move(o);
    }
    
    Logger::Logger(const std::string& filename, Verbosity v){
        out_ptr = unique_ptr<std::ostream>(new std::ofstream(filename, std::ios::out));
        verbosity = v;
    }

    std::ostream& Logger::get_stream() const {
        if (use_stdout) return std::cout;
        if (use_stderr) return std::cerr;
        return *out_ptr;
    }

    void Logger::redirect_to_file(const std::string& filename){
        if (filename.empty())
            return;

        if (use_stdout || use_stderr){
            use_stdout = false;
            use_stderr = false;
        }

        out_ptr.reset(new std::ofstream(filename, std::ios::out));
    }

    void Logger::redirect_to_stdout(){
        use_stdout = true;
        use_stderr = false;
        out_ptr.reset();
    }

    void Logger::redirect_to_stderr(){
        use_stdout = false;
        use_stderr = true;
        out_ptr.reset();
    }
}}
