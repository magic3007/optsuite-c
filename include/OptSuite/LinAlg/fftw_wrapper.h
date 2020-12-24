/*
 * ==========================================================================
 *
 *       Filename:  fftw_wrapper.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/26/2020 01:29:11 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#ifndef OPTSUITE_LINALG_FFTW_WRAPPER_H
#define OPTSUITE_LINALG_FFTW_WRAPPER_H

#include <iostream>
#include <fftw3.h>
#include <complex>
#include "OptSuite/core_n.h"

#if OPTSUITE_SCALAR_TOKEN == 0
#define OPTSUITE_FFTW(name) fftw_ ## name
#elif OPTSUITE_SCALAR_TOKEN == 1
#define OPTSUITE_FFTW(name) fftwf_ ## name
#else
#error "unsupported fftw type"
#endif

namespace OptSuite { namespace LinAlg {
    inline OPTSUITE_FFTW(complex) * fftw_cast(const std::complex<Scalar> *p){ 
        return const_cast<OPTSUITE_FFTW(complex)*>( 
                reinterpret_cast<const OPTSUITE_FFTW(complex)*>(p) ); 
    }

    inline Scalar* fftw_cast(const Scalar *p){
        return const_cast<Scalar*>(p);
    }

    class FFTWPlan {
        using ctype = OPTSUITE_FFTW(complex);
        OPTSUITE_FFTW(plan) plan;

        public:
            inline FFTWPlan() : plan(NULL) {}
            inline ~FFTWPlan(){
                if (plan != NULL)
                    OPTSUITE_FFTW(destroy_plan)(plan);
            }

            inline void execute(){
                if (plan != NULL)
                    OPTSUITE_FFTW(execute)(plan);
            }

            inline void forward(Index nfft, ctype *in, ctype *out){
                if (plan == NULL)
                    plan = OPTSUITE_FFTW(plan_dft_1d)(nfft, in, out, FFTW_FORWARD,
                            FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
                OPTSUITE_FFTW(execute_dft)(plan, in, out);
            }

            inline void forward_many(Index nfft, Index howmany, Index idist, Index odist,
                                     ctype *in, ctype *out){
                int n = nfft;
                if (plan == NULL){
                    plan = OPTSUITE_FFTW(plan_many_dft)(1, &n, howmany, in, &n, 1, idist, out, &n, 1, odist, FFTW_FORWARD, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
                }
                OPTSUITE_FFTW(execute_dft)(plan, in, out);
            }

            inline void forward(Index nfft, ComplexScalar *in, ComplexScalar *out){
                this->forward(nfft, fftw_cast(in), fftw_cast(out));
            }

            inline void forward2(Index n0, Index n1, ctype *in, ctype *out){
                if (plan == NULL)
                    plan = OPTSUITE_FFTW(plan_dft_2d)(n0, n1, in, out, FFTW_FORWARD,
                            FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
                OPTSUITE_FFTW(execute_dft)(plan, in, out);
            }

            inline void backward(Index nfft, ctype *in, ctype *out){
                if (plan == NULL)
                    plan = OPTSUITE_FFTW(plan_dft_1d)(nfft, in, out, FFTW_BACKWARD,
                            FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
                OPTSUITE_FFTW(execute_dft)(plan, in, out);
            }

            inline void backward_many(Index nfft, Index howmany, Index idist, Index odist,
                                     ctype *in, ctype *out){
                int n = nfft;
                if (plan == NULL){
                    plan = OPTSUITE_FFTW(plan_many_dft)(1, &n, howmany, in, &n, 1, idist, out, &n, 1, odist, FFTW_BACKWARD, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
                }
                OPTSUITE_FFTW(execute_dft)(plan, in, out);
            }

            inline void backward(Index nfft, ComplexScalar *in, ComplexScalar *out){
                this->backward(nfft, fftw_cast(in), fftw_cast(out));
            }

            inline void backward2(Index n0, Index n1, ctype *in, ctype *out){
                if (plan == NULL)
                    plan = OPTSUITE_FFTW(plan_dft_2d)(n0, n1, in, out, FFTW_BACKWARD,
                            FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
                OPTSUITE_FFTW(execute_dft)(plan, in, out);
            }


    };

    class FFTWManager {
        using ctype = OPTSUITE_FFTW(complex);
        std::map<uint64_t, FFTWPlan> map_dft_1d;

        FFTWPlan& get_plan(Index, bool, const void *, void *);

        public:
            inline void clear() { map_dft_1d.clear(); }
            void forward(const std::vector<ComplexScalar>&,
                               std::vector<ComplexScalar>&,
                               Index = -1);
            void backward(const std::vector<ComplexScalar>&,
                                std::vector<ComplexScalar>&,
                                Index = -1);

            void forward(const Ref<const CMat>, Ref<CMat>, Index = -1);
            void backward(const Ref<const CMat>, Ref<CMat>, Index = -1);

    };

    CMat fft(const Ref<const CMat>, Index = -1);
    CMat ifft(const Ref<const CMat>, Index = -1);

}}

#endif
