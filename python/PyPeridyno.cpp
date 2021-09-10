#include "PyPeridyno.h"

#include <Log.h>
using Log = dyno::Log;

void pybind_log(py::module& m)
{
	py::class_<Log>(m, "Log")
		.def(py::init<>())
		.def_static("set_output", &Log::setOutput)
		.def_static("get_output", &Log::getOutput)
		.def_static("send_message", &Log::sendMessage)
		.def_static("set_level", &Log::setLevel);
}

PYBIND11_MODULE(PyPeridyno, m) 
{
	m.doc() = "Python binding of Peridyno";

	pybind_core(m);
	pybind_log(m);

	pybind_framework(m);
	pybind_particle_system(m);
	pybind_rendering(m);
}