/*
 * ===========================================================================
 *
 *       Filename:  shared_spmat.h
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  12/12/2020 10:38:48 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, Peking University
 *
 * ===========================================================================
 */

#ifndef OPTSUITE_BASE_SHARED_SPMAT_H
#define OPTSUITE_BASE_SHARED_SPMAT_H

#include <vector>
#include <memory>
#include "OptSuite/core_n.h"
#include "OptSuite/Base/variable.h"

namespace OptSuite { namespace Base {
    template<typename dtype>
    class SharedSpMat : public Variable<dtype> {
        using spmat_t = Eigen::SparseMatrix<dtype, ColMajor, SparseIndex>;
        std::vector<dtype> data_;
        std::shared_ptr<std::vector<SparseIndex>> outer_;
        std::shared_ptr<std::vector<SparseIndex>> inner_;
        public:
            SharedSpMat() = default;
            SharedSpMat(Index, Index);
            SharedSpMat& operator=(const SharedSpMat&);
            Variable<dtype>& operator=(const Variable<dtype>&);

            Index rows() const;
            Index cols() const;
            Index nonZeros() const;

            void resize(Index, Index);
            void reserve(Index);

            const SparseIndex* outerIndexPtr() const;
            SparseIndex* outerIndexPtr();

            const SparseIndex* innerIndexPtr() const;
            SparseIndex* innerIndexPtr();

            const Scalar* valuePtr() const;
            Scalar* valuePtr();

            Map<const spmat_t> spmat() const;
            Map<spmat_t> spmat();
    };
}}

#endif

