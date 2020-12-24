/*
 * ==========================================================================
 *
 *       Filename:  rng_wrapper.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/06/2020 07:54:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */


#include "OptSuite/core_n.h"
#include "OptSuite/LinAlg/rng_wrapper.h"

namespace OptSuite { namespace LinAlg {
    namespace {
        std::mt19937 generator{std::random_device{}()};
    }

    Mat randn(Size m, Size n, Scalar mean, Scalar std){
        std::normal_distribution<Scalar> dist(mean, std);
        auto normal = [&] () { return dist(generator); };
        Mat x = Mat::NullaryExpr(m, n, normal);
        return x;
    }

    Mat rand(Size m, Size n, Scalar lower, Scalar upper){
        std::uniform_real_distribution<Scalar> dist(lower, upper);
        auto unif_r = [&] () { return dist(generator); };
        Mat x = Mat::NullaryExpr(m, n, unif_r);
        return x;
    }

    SpMat sprandn(Size m, Size n, Scalar density){
        OPTSUITE_ASSERT(density > 0 && density < 1);
        Size nnz_max = Size(n * m * density);
        std::normal_distribution<Scalar> dist(0, 1);
        std::uniform_int_distribution<Index> dist_i(0, m - 1);
        std::uniform_int_distribution<Index> dist_j(0, n - 1);

        std::set<std::pair<Index, Index>> ij;

        for (int i = 0; i < nnz_max; ++i)
            ij.insert(std::pair<Index, Index>(dist_i(generator), dist_j(generator)));

        std::vector<Triplet> tList;
        tList.reserve(nnz_max);

        for (auto it = ij.begin(); it != ij.end(); ++it){
            tList.push_back(Triplet(it->first, it->second, dist(generator)));
        }
                    
        SpMat mat(m, n);
        mat.setFromTriplets(tList.begin(), tList.end());

        return mat;

    }

    SpMat sprandn_c(Size m, Size n, Scalar density){
        OPTSUITE_ASSERT(density > 0 && density < 1);
        Size nnz_max_c = Size(m * density);
        Size nnz_max = nnz_max_c * n;
        std::normal_distribution<Scalar> dist(0, 1);
        std::uniform_int_distribution<Index> dist_i(0, m - 1);

        std::set<std::pair<Index, Index>> ij;

        for (int i = 0; i < nnz_max_c; ++i){
            Index ii = dist_i(generator);
            for (int j = 0; j < n; ++j)
                ij.insert(std::pair<Index, Index>(ii, j));
        }

        std::vector<Triplet> tList;
        tList.reserve(nnz_max);

        for (auto it = ij.begin(); it != ij.end(); ++it){
            tList.push_back(Triplet(it->first, it->second, dist(generator)));
        }
                    
        SpMat mat(m, n);
        mat.setFromTriplets(tList.begin(), tList.end());

        return mat;

    }

    const std::mt19937& get_generator(){
        return generator;
    }

    void rng(unsigned long seed){
        generator.seed(seed);
    }
}}


