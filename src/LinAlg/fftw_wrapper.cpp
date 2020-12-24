/*
 * ==========================================================================
 *
 *       Filename:  fftw_wrapper.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/26/2020 07:04:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#include "OptSuite/core_n.h"
#include "OptSuite/LinAlg/fftw_wrapper.h"


namespace OptSuite { namespace LinAlg { 
    FFTWPlan& FFTWManager::get_plan(Index nfft, bool inverse, const void * in, void * out){
        bool inplace = (in == out);
        bool aligned = ( (reinterpret_cast<size_t>(in)&15) | (reinterpret_cast<size_t>(out)&15) ) == 0;
        uint64_t key = ( (nfft<<3 ) | (inverse<<2) | (inplace<<1) | aligned ) << 1;
        return map_dft_1d[key];
    }
    void FFTWManager::forward(const std::vector<ComplexScalar>& in,
                                    std::vector<ComplexScalar>& out,
                                    Index nfft){
        if (nfft == -1) nfft = in.size();
        if (out.size() < (size_t)nfft) out.resize(nfft);
        get_plan(nfft, false, in.data(), out.data()).
            forward(nfft, fftw_cast(in.data()), fftw_cast(out.data()));
    }
    void FFTWManager::backward(const std::vector<ComplexScalar>& in,
                                     std::vector<ComplexScalar>& out,
                                     Index nfft){
        if (nfft == -1) nfft = in.size();
        if (out.size() < (size_t)nfft) out.resize(nfft);
        get_plan(nfft, true, in.data(), out.data()).
            backward(nfft, fftw_cast(in.data()), fftw_cast(out.data()));
    }

    void FFTWManager::forward(const Ref<const CMat> in, Ref<CMat> out, Index nfft){
        if (nfft == -1) nfft = in.rows();
        Index howmany = in.cols();
        Index idist = in.outerStride(), odist = out.outerStride();
        CMat tmp;

        auto in_ptr = in.data();
        auto out_ptr = out.data();

        // check whether we need a temporary mat
        if (in.innerStride() != 1 || in.rows() < nfft){
            tmp.resize(nfft, howmany);
            tmp.setZero();
            tmp.block(0, 0, std::min(in.rows(), nfft), howmany) = 
                in.block(0, 0, std::min(in.rows(), nfft), howmany);
            idist = tmp.outerStride();
            in_ptr = tmp.data();
        }

        if (howmany == 1){
            get_plan(nfft, false, in_ptr, out_ptr).
                forward(nfft, fftw_cast(in_ptr), fftw_cast(out_ptr));
        } else {
            // use temporary plan
            FFTWPlan fft_many;
            fft_many.forward_many(nfft, howmany, idist, odist, fftw_cast(in_ptr), fftw_cast(out_ptr));
        }

    }
    void FFTWManager::backward(const Ref<const CMat> in, Ref<CMat> out, Index nfft){
        if (nfft == -1) nfft = in.rows();
        Index howmany = in.cols();
        Index idist = in.outerStride(), odist = out.outerStride();
        CMat tmp;

        auto in_ptr = in.data();
        auto out_ptr = out.data();

        // check whether we need a temporary mat
        if (in.innerStride() != 1 || in.rows() < nfft){
            tmp.resize(nfft, howmany);
            tmp.setZero();
            tmp.block(0, 0, std::min(in.rows(), nfft), howmany) = 
                in.block(0, 0, std::min(in.rows(), nfft), howmany);
            idist = tmp.outerStride();
            in_ptr = tmp.data();
        }

        if (howmany == 1){
            get_plan(nfft, true, in_ptr, out_ptr).
                backward(nfft, fftw_cast(in_ptr), fftw_cast(out_ptr));
        } else {
            // use temporary plan
            FFTWPlan fft_many;
            fft_many.backward_many(nfft, howmany, idist, odist, fftw_cast(in_ptr), fftw_cast(out_ptr));
        }

        // scaling
#ifndef OPTSUITE_UNSCALED_IFFT
        out /= (Scalar)nfft;
#endif

    }

    namespace {
        static FFTWManager manager;
    }

    CMat fft(const Ref<const CMat> in, Index nfft){
        if (nfft == -1) nfft = in.rows();
        CMat out(nfft, in.cols());
        manager.forward(in, out, nfft);
        return out;
    }

    CMat ifft(const Ref<const CMat> in, Index nfft){
        if (nfft == -1) nfft = in.rows();
        CMat out(nfft, in.cols());
        manager.backward(in, out, nfft);
        return out;
    }


}}
