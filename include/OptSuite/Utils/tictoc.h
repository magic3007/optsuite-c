#ifndef OPTSUITE_UTILS_TICTOC_H
#define OPTSUITE_UTILS_TICTOC_H

#include <chrono>
#include "OptSuite/core_n.h"

// usage:
// auto tstart = tic();
// auto duration = toc(tstart);
 
// By default, it returns a Scalar in seconds. 
// This can be adjusted in the template parameter of toc.

namespace OptSuite { namespace Utils {

    using ClockType = std::chrono::steady_clock;
    using TicReturnType = std::chrono::time_point<ClockType>;

    // different from MATLAB tic/toc in that toc requires the result of tic
    // so issue a warning if the result is not used
#if (__cplusplus >= 201703L)
    [[nodiscard]] 
#endif
    inline
    TicReturnType tic() {
        return ClockType::now();
    }

    template <typename DurationType = std::chrono::duration<Scalar>>
    inline
    typename DurationType::rep toc(const TicReturnType& tstart) {
        DurationType duration = ClockType::now() - tstart;
        return duration.count();
    }
}}

#endif
