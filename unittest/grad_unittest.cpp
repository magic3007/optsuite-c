/**
 * grad_unittest.cpp
 * Created by Jing Mai on 01/12/2021.
 */
#include "OptSuite/Base/functional.h"
#include "OptSuite/LinAlg/rng_wrapper.h"
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

class LogisticRegressionTest : public TestWithParam<std::tuple<int32_t, int32_t>> {
protected:
    void SetUp() override {
        int32_t m, n;
        std::tie(m, n) = GetParam();
        A              = randn(n, m);
        b              = randn(m, 1);
        x              = randn(n, 1);
        functor        = new LogisticRegression<Scalar>(A, b);
    }
    void TearDown() {
        delete functor;
        functor = nullptr;
    };
    Mat                         A, x, b;
    LogisticRegression<Scalar> *functor;
};

TEST_P(AxmbNormSqrTest, ForwardAndBackWard) {
    Mat    grad_x        = Mat(x_.rows(), x_.cols());
    Scalar y             = (*functor_)(x_, grad_x, /* grad */ true);
    Mat    r             = A_ * x_ - b_;
    Scalar y_ground      = 0.5 * r.squaredNorm();
    Mat    grad_x_ground = A_.transpose() * r;
    EXPECT_TRUE(std::fabs(y - y_ground) < eps);
    EXPECT_TRUE(grad_x_ground.isApprox(grad_x));
}

TEST_P(LogisticRegressionTest, ForwardAndBackWard) {
    Mat    grad_x = Mat(x.rows(), x.cols());
    Scalar y      = (*functor)(x, grad_x, true);
//    std::cout << y << std::endl;
//    std::cout << grad_x.squaredNorm() << std::endl;
}

INSTANTIATE_TEST_SUITE_P(XIsMat, AxmbNormSqrTest, Combine(Values(512), Values(256), Values(1, 2)));

INSTANTIATE_TEST_SUITE_P(XIsMat, LogisticRegressionTest, Combine(Values(512), Values(256)));

}   // namespace