#include "pybind11/eigen.h"
#include "pybind11/pybind11.h"

namespace py = pybind11;
using namespace pybind11::literals;

template<typename... Args>
using overload_cast_ = pybind11::detail::overload_cast_impl<Args...>;

#include "OptSuite/Base/functional.h"


using namespace OptSuite;
using namespace OptSuite::Base;

template<typename dtype>
class PyFunc : public Func<dtype> {
    using typename Func<dtype>::mat_t;
    using Func<dtype>::Func;   // Inherit constructors
    Scalar operator()(const Ref<const mat_t> x) override {
        PYBIND11_OVERRIDE_NAME(Scalar, Func<dtype>, "__call__", operator(), x);
    }
};

template<typename dtype>
class PyProximal : public Proximal<dtype> {
    using typename Proximal<dtype>::mat_t;
    using Proximal<dtype>::Proximal;
    void operator()(const Ref<const mat_t> x, Scalar t, Ref<mat_t> y) override {
        PYBIND11_OVERRIDE_NAME(void, Proximal<dtype>, "__call__", operator(), x, t, y);
    }
};

template<typename dtype>
class PyFuncGrad : public FuncGrad<dtype> {
    using typename FuncGrad<dtype>::mat_t;
    using FuncGrad<dtype>::FuncGrad;
    Scalar operator()(const Ref<const mat_t> x) override {
        PYBIND11_OVERRIDE_NAME(Scalar, FuncGrad<dtype>, "__call__", operator(), x);
    }
    Scalar operator()(const Ref<const mat_t> x, Ref<mat_t> y, bool compute_grad) override {
        PYBIND11_OVERRIDE_NAME(Scalar, FuncGrad<dtype>, "__call__", operator(), x, y, compute_grad);
    }
};


template<typename dtype>
class PyAxmbNormSqr : public AxmbNormSqr<dtype> {
    using typename AxmbNormSqr<dtype>::mat_t;
    using AxmbNormSqr<dtype>::AxmbNormSqr;
    Scalar operator()(const Ref<const mat_t> x, Ref<mat_t> y, bool compute_grad) override {
        PYBIND11_OVERRIDE_NAME(Scalar, AxmbNormSqr<dtype>, "__call__", operator(), x, y,
                               compute_grad);
    }
};


template<typename dtype>
class PyLogisticRegression : public LogisticRegression<dtype> {
    using typename LogisticRegression<dtype>::mat_t;
    using LogisticRegression<dtype>::LogisticRegression;
    Scalar operator()(const Ref<const mat_t> x, Ref<mat_t> y, bool compute_grad) override {
        PYBIND11_OVERRIDE_NAME(Scalar, LogisticRegression<dtype>, "__call__", operator(), x, y,
                               compute_grad);
    }
};


class PyShrinkageL1 : public ShrinkageL1 {
    using ShrinkageL1::ShrinkageL1;
    using typename ShrinkageL1::mat_t;
    void operator()(const Ref<const mat_t> x, Scalar t, Ref<mat_t> y) override {
        PYBIND11_OVERRIDE_NAME(void, ShrinkageL1, "__call__", operator(), x, t, y);
    }
};

template<typename dtype>
static void DeclareFunc(py::module &m, const std::string &type_str) {
    using Class              = Func<dtype>;
    using PyClass            = PyFunc<dtype>;
    using mat_t              = typename Class::mat_t;
    std::string pyclass_name = "Func_" + type_str;
    py::class_<Class, PyClass>(m, pyclass_name.c_str())
            .def(py::init<>())
            .def("__call__", overload_cast_<const Ref<const mat_t>>()(&Class::operator()),
                 "Functor for mat_t");
}

template<typename dtype>
static void DeclareProximal(py::module &m, const std::string &type_str) {
    using Class              = Proximal<dtype>;
    using PyClass            = PyProximal<dtype>;
    using mat_t              = typename Class::mat_t;
    std::string pyclass_name = "Proximal_" + type_str;
    py::class_<Class, PyClass>(m, pyclass_name.c_str())
            .def(py::init<>())
            .def("__call__",
                 overload_cast_<const Ref<const mat_t>, Scalar, Ref<mat_t>>()(&Class::operator()),
                 "Functor for mat_t");
}

template<typename dtype>
static void DeclareFuncGrad(py::module &m, const std::string &type_str) {
    using Class              = FuncGrad<dtype>;
    using PyClass            = PyFuncGrad<dtype>;
    using mat_t              = typename Class::mat_t;
    std::string pyclass_name = "FuncGrad_" + type_str;
    py::class_<Class, PyClass>(m, pyclass_name.c_str())
            .def(py::init<>())
            .def("__call__", overload_cast_<const Ref<const mat_t>>()(&Class::operator()))
            .def("__call__",
                 overload_cast_<const Ref<const mat_t>, Ref<mat_t>, bool>()(&Class::operator()));
}

template<typename dtype>
static void DeclareAxmbNormSqr(py::module &m, const std::string &type_str) {
    using Class              = AxmbNormSqr<dtype>;
    using PyClass            = PyAxmbNormSqr<dtype>;
    using mat_t              = typename Class::mat_t;
    std::string pyclass_name = "AxmbNormSqr_" + type_str;
    py::class_<Class, PyClass>(m, pyclass_name.c_str())
            .def(py::init<const Ref<const mat_t>, const Ref<const mat_t>>())
            .def("__call__",
                 overload_cast_<const Ref<const mat_t>, Ref<mat_t>, bool>()(&Class::operator()));
}

template<typename dtype>
static void DeclareLogisticRegression(py::module &m, const std::string &type_str) {
    using Class              = LogisticRegression<dtype>;
    using PyClass            = PyLogisticRegression<dtype>;
    using mat_t              = typename Class::mat_t;
    std::string pyclass_name = "LogisticRegression_" + type_str;
    py::class_<Class, PyClass>(m, pyclass_name.c_str())
            .def(py::init<Ref<const mat_t>, Ref<const mat_t>>())
            .def("__call__",
                 overload_cast_<Ref<const mat_t>, Ref<mat_t>, bool>()(&Class::operator()));
}

static void DeclareShrinkageL1(py::module &m, const std::string &type_str) {
    using Class              = ShrinkageL1;
    using PyClass            = PyShrinkageL1;
    using mat_t              = typename Class::mat_t;
    std::string pyclass_name = "ShrinkageL1_" + type_str;
    py::class_<Class, PyClass>(m, pyclass_name.c_str())
            .def(py::init<Scalar>())
            .def("__call__",
                 overload_cast_<const Ref<const mat_t>, Scalar, Ref<mat_t>>()(&Class::operator()));
}

PYBIND11_MODULE(functional, m) {
    static_assert(std::is_same<double, Scalar>::value,
                  "The package only support data type `double`.");
    py::class_<Functional>(m, "Functional").def(py::init<>());

    DeclareFunc<double>(m, "float64");

    DeclareProximal<double>(m, "float64");

    DeclareFuncGrad<double>(m, "float64");

    DeclareAxmbNormSqr<double>(m, "float64");

    DeclareLogisticRegression<double>(m, "float64");

    DeclareShrinkageL1(m, "float64");
}