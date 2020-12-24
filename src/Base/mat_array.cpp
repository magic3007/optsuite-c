/*
 * ===========================================================================
 *
 *       Filename:  mat_array.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/13/2020 09:31:29 PM
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
#include "OptSuite/Base/mat_array.h"

namespace OptSuite { namespace Base {
    template<typename dtype>
    MatArray_t<dtype>::MatArray_t(const MatArray_t<dtype>& other){
        this->_offset = other._offset;
        this->_block_info = other._block_info;
        this->_data = other._data;
    }

    template<typename dtype>
    MatArray_t<dtype>& MatArray_t<dtype>::operator=(const MatArray_t<dtype>& other){
        this->_offset = other._offset;
        this->_block_info = other._block_info;
        this->_data = other._data;
        return *this;
    }

    template<typename dtype>
    Variable<dtype>& MatArray_t<dtype>::operator=(const Variable<dtype>& other){
        const MatArray_t<dtype>* other_ptr = dynamic_cast<const MatArray_t<dtype>*>(&other);
        OPTSUITE_ASSERT(other_ptr);
        return this->operator=(*other_ptr);
    }

    template<typename dtype>
    void MatArray_t<dtype>::add_blocks(Size m, Size n, Size count) {
        this->_blocks_it.clear();
        for (int i = 0; i < count; ++i){
            this->_offset.push_back(total_size());
            this->_data.resize(total_size() + m * n, 0);
            this->_block_info.push_back(std::pair<Size,Size>(m, n));
        }
    }

    template<typename dtype>
    void MatArray_t<dtype>::reserve(Size s){
        this->_data.reserve(s);
    }

    template<typename dtype>
    void MatArray_t<dtype>::gen_blocks_it() const {
        if (!_blocks_it.empty())
            return;
        for (int i = 0; i < total_blocks(); ++i){
            _blocks_it.push_back((*this)[i]);
        }
    }

    template<typename dtype>
    Size MatArray_t<dtype>::total_size() const {
        return this->_data.size();
     }

    template<typename dtype>
    Size MatArray_t<dtype>::total_blocks() const {
        return this->_block_info.size();
    }

    template<typename dtype>
    Map<typename MatArray_t<dtype>::vec_t> MatArray_t<dtype>::vec() {
        OPTSUITE_ASSERT(total_size() > 0);
        return Map<vec_t>(_data.data(), total_size(), 1);
    }

    template<typename dtype>
    Map<const typename MatArray_t<dtype>::vec_t> MatArray_t<dtype>::vec() const {
        OPTSUITE_ASSERT(total_size() > 0);
        return Map<const vec_t>(_data.data(), total_size(), 1);
    }

    template<typename dtype>
    Map<typename MatArray_t<dtype>::mat_t> MatArray_t<dtype>::operator[](Index ind) {
        Index off = _offset[ind];
        return Map<mat_t>(_data.data() + off, _block_info[ind].first,
                _block_info[ind].second);
    }

    template<typename dtype>
    Map<const typename MatArray_t<dtype>::mat_t> MatArray_t<dtype>::operator[](Index ind) const {
        Index off = _offset[ind];
        return Map<const mat_t>(_data.data() + off, _block_info[ind].first,
                _block_info[ind].second);
    }

    template<typename dtype>
    typename MatArray_t<dtype>::iterator MatArray_t<dtype>::begin() const {
        gen_blocks_it();
        return _blocks_it.cbegin();
    }

    template<typename dtype>
    typename MatArray_t<dtype>::iterator MatArray_t<dtype>::end() const {
        gen_blocks_it();
        return _blocks_it.cend();
    }

    template<typename dtype>
    Scalar MatArray_t<dtype>::squaredNorm() const {
        return this->vec().squaredNorm();
    }

    template<typename dtype>
    Scalar MatArray_t<dtype>::norm() const {
        return this->vec().norm();
    }

    template<typename dtype>
    dtype MatArray_t<dtype>::dot(const MatArray_t<dtype>& other) const {
        return this->vec().dot(other.vec());
    }

    template<typename dtype>
    dtype MatArray_t<dtype>::dot(const Variable<dtype>& other) const {
        const MatArray_t<dtype>* other_ptr = dynamic_cast<const MatArray_t<dtype>*>(&other);
        OPTSUITE_ASSERT(other_ptr);
        return this->dot(*other_ptr);
    }

    template<typename dtype>
    Scalar MatArray_t<dtype>::squared_norm_diff(const Variable<dtype>& other) const {
        const MatArray_t<dtype>* other_ptr = dynamic_cast<const MatArray_t<dtype>*>(&other);
        OPTSUITE_ASSERT(other_ptr);
        return (this->vec() - other_ptr->vec()).squaredNorm();
    }

    template<typename dtype>
    void MatArray_t<dtype>::set_zero_like(const Variable<dtype>& other){
        const MatArray_t<dtype>* other_ptr = dynamic_cast<const MatArray_t<dtype>*>(&other);
        OPTSUITE_ASSERT(other_ptr);
        *this = MatArray_t<dtype>::zeros_like(*other_ptr);
    }

    template<typename dtype>
    MatArray_t<dtype> MatArray_t<dtype>::zeros_like(const MatArray_t<dtype>& other){
        MatArray_t<dtype> tmp;
        tmp._block_info = other._block_info;
        tmp._offset = other._offset;
        tmp._data.resize(other.total_size(), 0);
        return tmp;
    }

    template<typename dtype>
    MatArray_t<dtype> MatArray_t<dtype>::from_vec_like(const Ref<const vec_t> v, const MatArray_t<dtype>& bv){
        OPTSUITE_ASSERT(bv.total_size() == v.size());
        MatArray_t<dtype> tmp = MatArray_t<dtype>::zeros_like(bv);
        tmp.vec() = v;
        return tmp;
    }

    template<typename dtype>
    std::ostream& operator<<(std::ostream& out, const MatArray_t<dtype>& bv){
        for (auto&& i = bv.begin(); i != bv.end(); ++i){
            Index ind = std::distance(bv.begin(), i);
            out << "[" << ind <<"]" << std::endl;
            out << *i << std::endl;

            if (ind != bv.total_blocks() - 1)
                out << std::endl;
        }
        return out;
    }

    // template instatiation
    template class MatArray_t<Scalar>;
    template class MatArray_t<ComplexScalar>;
    template std::ostream& operator<<(std::ostream&, const MatArray_t<Scalar>&);
    template std::ostream& operator<<(std::ostream&, const MatArray_t<ComplexScalar>&);
}}

