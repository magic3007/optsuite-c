/*
 * ==========================================================================
 *
 *       Filename:  optionlist.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/20/2020 02:06:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#ifndef OPTSUITE_UTILS_OPTIONLIST_H
#define OPTSUITE_UTILS_OPTIONLIST_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "OptSuite/core_n.h"
#include "OptSuite/Utils/logger.h"
#include "OptSuite/Utils/optionchecker.h"

#define OPTSUITE_OPTION_CMD_PREFIX "--opt"

namespace OptSuite { namespace Utils {
    class OptionList;
    enum class OptionType {
        Unknown,
        String,
        Integer,
        Scalar,
        Bool
    };

    struct OptionValue {
        std::string s;
        Scalar d;
        Index i;
        bool b;
    };


    class RegOption {
        friend OptionList;
        friend std::ostream& operator<<(std::ostream&, const OptionList&);

        using OptionChecker_ptr = std::shared_ptr<OptionChecker>;

        OptionType type;
        std::string tag;
        OptionValue default_value;
        std::shared_ptr<OptionChecker> checker_ptr;
        std::string description;

        public:
            RegOption();

            RegOption(const std::string&, const std::string&, const std::string&,
                      OptionChecker_ptr = std::make_shared<OptionChecker>());
            RegOption(const std::string&, const char *, const std::string&,
                      OptionChecker_ptr = std::make_shared<OptionChecker>());
            RegOption(const std::string&, const Index&, const std::string&,
                      OptionChecker_ptr = std::make_shared<OptionChecker>());
            RegOption(const std::string&, const Scalar&, const std::string&,
                      OptionChecker_ptr = std::make_shared<OptionChecker>());
            RegOption(const std::string&, const bool&, const std::string&,
                      OptionChecker_ptr = std::make_shared<OptionChecker>());

            std::string to_string_v() const;
            static std::string to_string_v(OptionType, const OptionValue&);

    };

    class OptionList {
        using string_map = std::unordered_map<std::string, OptionValue>;
        using regoption_map = std::unordered_map<std::string, RegOption>;
        public:
            OptionList() = default;
            OptionList(std::vector<RegOption>&, std::shared_ptr<Logger> = nullptr);
            ~OptionList() = default;

            void register_option(const RegOption&);
            inline bool is_initialized(const std::string& tag) const {
                return options.find(tag) != options.cend();
            }
            
            template<typename T>
            bool check_option_t(const std::string&, const T&, const OptionType, const bool = true);
            bool set_integer(const std::string&, const Index&);
            bool set_string(const std::string&, const std::string&);
            bool set_scalar(const std::string&, const Scalar&);
            bool set_bool(const std::string&, const bool&);

            std::string get_string(const std::string&);
            Index get_integer(const std::string&);
            Scalar get_scalar(const std::string&);
            bool get_bool(const std::string&);

            void set_from_file(const std::string&);
            void set_from_string(const std::string&, const std::string&);
            void set_from_cmd_line(int, char *[]);

            void show(bool = false) const;
            void show(std::ostream&, bool = false) const;
            friend std::ostream& operator<<(std::ostream&, const OptionList&);

        private:
            string_map options;
            regoption_map reg_options;
            std::shared_ptr<Logger> logger_ptr;

            static std::string to_string(OptionType);
            std::string to_string_v(const std::string&) const;
            bool get_option_type(const std::string&, OptionType&);
    };

}}
#endif
