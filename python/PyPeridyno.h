#pragma once

#include "PyCommon.h"


void pybind_core(py::module& m); 
void pybind_log(py::module& m);
void pybind_framework(py::module& m);
void pybind_particle_system(py::module& m);
void pybind_rendering(py::module& m);