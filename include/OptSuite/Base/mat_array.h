/*
 * ===========================================================================
 *
 *       Filename:  mat_array.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/13/2020 09:08:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, Peking University
 *
 * ===========================================================================
 */

#ifndef OPTSUITE_BASE_MAT_ARRAY_H
#define OPTSUITE_BASE_MAT_ARRAY_H

#include <vector>
#include <iostream>
#include "OptSuite/core_n.h"
#include "OptSuite/Base/variable.h"


namespace OptSuite { namespace Base {
    template<typename dtype>
    class MatArray_t : public Variable<dtype>{
        using mat_t = Eigen::Matrix<dtype, Dynamic, Dynamic>;
        using vec_t = Eigen::Matrix<dtype, Dynamic, 1>;
        std::vector<dtype> _data;
        std::vector<std::pair<Size,Size>> _block_info;
        std::vector<Index> _offset;
        mutable std::vector<Map<const mat_t>> _blocks_it;

        void gen_blocks_it() const;

        public:
            using iterator = typename std::vector<Map<const mat_t>>::const_iterator;

            MatArray_t() = default;
            MatArray_t(const MatArray_t<dtype>&);
            MatArray_t<dtype>& operator=(const MatArray_t<dtype>&);
            Variable<dtype>& operator=(const Variable<dtype>&);
            Size total_size() const;
            Size total_blocks() const;
            void add_blocks(Size, Size, Size = 1);
            void reserve(Size);
            Map<vec_t> vec();
            Map<const vec_t> vec() const;
            Map<mat_t> operator[](Index);
            Map<const mat_t> operator[](Index) const;

            Scalar squaredNorm() const;
            Scalar norm() const;
            dtype dot(const MatArray_t<dtype>&) const;
            dtype dot(const Variable<dtype>&) const;
            void set_zero_like(const Variable<dtype>&);
            //void set_zero_like(const MatArray_t<dtype>&);

            inline bool has_squared_norm_diff() const { return true; }
            Scalar squared_norm_diff(const Variable<dtype>&) const;

            iterator begin() const;
            iterator end() const;

            static MatArray_t<dtype> zeros_like(const MatArray_t<dtype>&);
            static MatArray_t<dtype> from_vec_like(const Ref<const vec_t>, const MatArray_t<dtype>&);

            template<typename Q>
            friend std::ostream& operator<<(std::ostream&, const MatArray_t<Q>&);
    };

    using MatArray = MatArray_t<Scalar>;
    using CMatArray = MatArray_t<ComplexScalar>;
}}

#endif

