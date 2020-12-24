/*
 * ==========================================================================
 *
 *       Filename:  model.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/01/2020 05:51:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#ifndef OPTSUITE_BASE_MODEL_H
#define OPTSUITE_BASE_MODEL_H

#include <string>

namespace OptSuite { namespace Base {
    class ModelBase {
        public:
            inline ModelBase(std::string name_) : name(name_) {};
            ~ModelBase() = default;

            std::string name;
            
        private:
    };
}}

#endif

