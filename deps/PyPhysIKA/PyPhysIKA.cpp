#include "PyPhysIKA.h"

#include "PyCore.h"
#include "PyFramework.h"
#include "PyParticleSystem.h"
#include "PyRigidBody.h"
//#include "PyHeightField.h"
#include "PyRendering.h"

// void init_GlutGUI(py::module &);
// void init_Core(py::module &);

PYBIND11_MODULE(PyPhysIKA, m) {

	m.doc() = "Python binding of PhysIKA";

	pybind_core(m);
	pybind_framework(m);
	pybind_particle_system(m);
	pybind_rigid_body(m);
	//pybind_height_field(m);
	pybind_rendering(m);
}