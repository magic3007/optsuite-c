#include "pybind11/eigen.h"
#include "pybind11/pybind11.h"

namespace py = pybind11;
using namespace pybind11::literals;

template<typename... Args>
using overload_cast_ = pybind11::detail::overload_cast_impl<Args...>;

#include "OptSuite/Base/solver.h"

using namespace OptSuite;
using namespace OptSuite::Base;

static void BindStepSizeStrategy(py ::module &m) {
    py::enum_<StepSizeStrategy>(m, "StepSizeStrategy")
            .value("Fixed", StepSizeStrategy::Fixed)
            .export_values();

    py::class_<FixedStepSize>(m, "FixedStepSize")
            .def(py::init<Scalar>())
            .def("__call__", &FixedStepSize::operator());
}

static void BindSolverOptions(py::module &m) {
    py::class_<SolverOptions>(m, "SolverOptions")
            .def(py::init<>())
            .def_property("ftol", overload_cast_<>()(&SolverOptions::ftol),
                          overload_cast_<Scalar>()(&SolverOptions::ftol))
            .def_property("maxit", overload_cast_<>()(&SolverOptions::maxit),
                          overload_cast_<Index>()(&SolverOptions::maxit))
            .def_property("min_lasting_iters",
                          overload_cast_<>()(&SolverOptions::min_lasting_iters, py::const_),
                          overload_cast_<Index>()(&SolverOptions::min_lasting_iters))
            .def_property("verbosity", overload_cast_<>()(&SolverOptions::verbosity),
                          overload_cast_<Verbosity>()(&SolverOptions::verbosity))
            .def_property("step_size_strategy",
                          py::cpp_function(overload_cast_<>()(&SolverOptions::step_size_strategy),
                                           py::return_value_policy::reference),
                          overload_cast_<StepSizeStrategy>()(&SolverOptions::step_size_strategy))
            .def_property("fixed",
                          py::cpp_function(overload_cast_<>()(&SolverOptions::fixed, py::const_),
                                           py::return_value_policy::reference),
                          overload_cast_<const FixedStepSize &>()(&SolverOptions::fixed));
}

static void BindSolverRecords(py::module &m) {
    py::class_<SolverRecords>(m, "SolverRecords")
            .def(py::init<>())
            .def("get_n_iters", &SolverRecords::get_n_iters)
            .def("get_elapsed_time_us", &SolverRecords::get_elapsed_time_us);
}

static void BindSolver(py::module &m) {
    py::class_<SolverBase>(m, "SolverBase")
            .def(py::init<std::string>());

    py::class_<ProximalGradSolver, SolverBase>(m, "ProximalGradSolver")
            .def(py::init<std::string, SolverOptions>())
            .def("__call__", &ProximalGradSolver::operator());
}

PYBIND11_MODULE(solver, m) {
    BindStepSizeStrategy(m);
    BindSolverOptions(m);
    BindSolverRecords(m);
    BindSolver(m);
}
