#include <pybind11/pybind11.h>
#include "Optsuite/Base/solver.h"
#include "OptSuite/Base/functional.h"

namespace py = pybind11;
namespace OptSuite { namespace Interface {
    using namespace OptSuite::Base;
    PYBIND11_MODULE(pyoptsuite, m) {
        py::class_<SolverOptions> (m, "SolverOptions");
        py::class_<ProximalGradSolver> (m, "ProximalGradSolver")
            .def(py::init<std::string, SolverOptions>());
    }
}}