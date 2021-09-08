#include "PyRigidBody.h"

#include "RigidBody/RigidBody.h"

using namespace PhysIKA;

template <typename TDataType>
void declare_rigid_body(py::module& m, std::string typestr) {

	py::class_<RigidBody<TDataType>, Node, std::shared_ptr<RigidBody<TDataType>>>
		(m, ("RigidBody" + typestr).c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<>())
		.def("get_surface", &RigidBody<TDataType>::getSurface)
		.def("translate", &RigidBody<TDataType>::translate)
		.def("scale", &RigidBody<TDataType>::scale)
		.def("load_shape", &RigidBody<TDataType>::loadShape);
}


void pybind_rigid_body(py::module& m)
{
	declare_rigid_body<PhysIKA::DataType3f>(m, "3f");
}