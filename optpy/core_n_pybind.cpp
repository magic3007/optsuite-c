#include "pybind11/eigen.h"
#include "pybind11/pybind11.h"

namespace py = pybind11;
using namespace pybind11::literals;   // shorthand

#include "OptSuite/core_n.h"

using namespace OptSuite;

PYBIND11_MODULE(core_n, m) {
    py::enum_<Verbosity>(m, "Verbosity")
            .value("Quiet", Verbosity::Quiet)
            .value("Info", Verbosity::Info)
            .value("Debug", Verbosity::Debug)
            .value("Verbose", Verbosity::Verbose)
            .value("Everything", Verbosity::Everything)
            .export_values();

    m.attr("eps") = eps;
}