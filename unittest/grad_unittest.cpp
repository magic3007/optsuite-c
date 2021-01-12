/**
 * grad_unittest.cpp
 * Created by Jing Mai on 01/12/2021.
 */
#include "OptSuite/Base/functional.h"
#include "OptSuite/LinAlg/rng_wrapper.h"
#include "OptSuite/core_n.h"
#include "gtest/gtest.h"

namespace {

using ::testing::Bool;
using ::testing::Combine;
using ::testing::Range;
using ::testing::TestWithParam;
using ::testing::Values;

using namespace OptSuite;
using namespace OptSuite::Base;
using namespace OptSuite::LinAlg;
using namespace OptSuite::Utils;

class AxmbNormSqrTest : public TestWithParam<::std::tuple<int32_t, int32_t, int32_t>> {
protected:
    void SetUp() override {
        int32_t m, n, l;
        std::tie(m, n, l) = GetParam();
        A_                = randn(m, n);
        x_                = randn(n, l);
        b_                = randn(m, l);
        functor_          = new AxmbNormSqr<Scalar>(A_, b_);
    }
    void TearDown() override {
        delete functor_;
        functor_ = nullptr;
    }
    Mat                  A_, x_, b_;
    AxmbNormSqr<Scalar> *functor_;
};

TEST_P(AxmbNormSqrTest, Forward) {
    Mat    grad_x        = Mat(x_.rows(), x_.cols());
    Scalar y             = (*functor_)(x_, grad_x, /* grad */ true);
    Mat    r             = A_ * x_ - b_;
    Scalar y_ground      = 0.5 * r.squaredNorm();
    Mat    grad_x_ground = A_.transpose() * r;
    EXPECT_TRUE(std::fabs(y - y_ground) < eps);
    EXPECT_TRUE(grad_x_ground.isApprox(grad_x));
}

INSTANTIATE_TEST_SUITE_P(XIsMat, AxmbNormSqrTest, Combine(Values(512), Values(256), Values(1, 2)));

}   // namespace