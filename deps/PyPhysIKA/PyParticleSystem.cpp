#include "PyParticleSystem.h"

#include <Dynamics/ParticleSystem/ParticleElasticBody.h>
#include <Dynamics/ParticleSystem/ParticleElastoPlasticBody.h>
#include <Dynamics/ParticleSystem/ParticleFluid.h>
#include <Dynamics/ParticleSystem/ParticleRod.h>

#include <Dynamics/ParticleSystem/StaticBoundary.h>
#include <Dynamics/ParticleSystem/ElasticityModule.h>
#include <Dynamics/ParticleSystem/ElastoplasticityModule.h>
#include <Dynamics/ParticleSystem/GranularModule.h>
#include <Dynamics/ParticleSystem/FractureModule.h>
#include <Dynamics/ParticleSystem/HyperelasticityModule.h>
#include <Dynamics/ParticleSystem/SolidFluidInteraction.h>
#include <Dynamics/ParticleSystem/MultipleFluidModel.h>
#include <Dynamics/ParticleSystem/PositionBasedFluidModel.h>
#include <Dynamics/RigidBody/RigidBody.h>

#include <Framework/Mapping/PointSetToPointSet.h>

#include "ParticleViscoplasticBody.h"

template <typename TDataType>
void declare_static_boundary(py::module &m, std::string typestr) {
	using Class = PhysIKA::StaticBoundary<TDataType>;
	using Parent = PhysIKA::Node;
	std::string pyclass_name = std::string("StaticBoundary") + typestr;
	py::class_<Class, Parent, std::shared_ptr<Class>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<>())
		.def("add_rigid_body", &Class::addRigidBody)
		.def("add_particle_system", &Class::addParticleSystem)
		.def("load_sdf", &Class::loadSDF)
		.def("load_cube", &Class::loadCube)
		.def("load_sphere", &Class::loadShpere)
		.def("translate", &Class::translate)
		.def("scale", &Class::scale);
}


template <typename TDataType>
void declare_particle_system(py::module &m, std::string typestr) {
	using Class = PhysIKA::ParticleSystem<TDataType>;
	using Parent = PhysIKA::Node;
	std::string pyclass_name = std::string("ParticleSystem") + typestr;

	py::class_<Class, Parent, std::shared_ptr<Class>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def("load_particles", (void (Class::*)(std::string)) & Class::loadParticles)
		.def("load_particles", (void (Class::*)(Class::Coord, Class::Coord, Real))&Class::loadParticles)
		.def("translate", &Class::translate)
		.def("scale", &Class::scale)
		.def("current_position", &Class::currentPosition, py::return_value_policy::reference)
		.def("current_velocity", &Class::currentVelocity, py::return_value_policy::reference)
		.def("current_force", &Class::currentForce, py::return_value_policy::reference)
		.def(py::init<>());
}

template <typename TDataType>
void declare_particle_elastic_body(py::module &m, std::string typestr) {

	py::class_<PhysIKA::PointSetToPointSet<TDataType>, std::shared_ptr<PhysIKA::PointSetToPointSet<TDataType>>>(m, "PointSetToPointSet3f")
		.def(py::init<>())
		.def("set_searching_radius", &PhysIKA::PointSetToPointSet<TDataType>::setSearchingRadius);

	using Class = PhysIKA::ParticleElasticBody<TDataType>;
	using Parent = PhysIKA::ParticleSystem<TDataType>;
	std::string pyclass_name = std::string("ParticleElasticBody") + typestr;
	py::class_<Class, Parent, std::shared_ptr<Class>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<>())
		.def(py::init<std::string>())
		.def("load_surface", &Class::loadSurface)
		.def("get_surface_node", &Class::getSurfaceNode)
		.def("set_elasticity_solver", &Class::setElasticitySolver)
		.def("get_elasticity_solver", &Class::getElasticitySolver)
		.def("get_topology_mapping", &Class::getTopologyMapping);
}

template <typename TDataType>
void declare_particle_elastoplastic_body(py::module& m, std::string typestr) {
	using Class = PhysIKA::ParticleElastoplasticBody<TDataType>;
	using Parent = PhysIKA::ParticleSystem<TDataType>;
	std::string pyclass_name = std::string("ParticleElastoplasticBody") + typestr;
	py::class_<Class, Parent, std::shared_ptr<Class>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<>())
		.def(py::init<std::string>())
		.def("load_surface", &Class::loadSurface)
		.def("set_elastoplasticity_solver", &Class::setElastoplasticitySolver)
		.def("get_surface_node", &Class::getSurfaceNode); 
}

template <typename TDataType>
void declare_particle_viscoplastic_body(py::module& m, std::string typestr) {
	using Class = PhysIKA::ParticleViscoplasticBody<TDataType>;
	using Parent = PhysIKA::ParticleSystem<TDataType>;
	std::string pyclass_name = std::string("ParticleViscoplasticBody") + typestr;
	py::class_<Class, Parent, std::shared_ptr<Class>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<>())
		.def(py::init<std::string>())
		.def("load_surface", &Class::loadSurface);
}


template <typename TDataType>
void declare_particle_fluid(py::module& m, std::string typestr) {
	using Class = PhysIKA::ParticleFluid<TDataType>;
	using Parent = PhysIKA::ParticleSystem<TDataType>;
	std::string pyclass_name = std::string("ParticleFluid") + typestr;
	py::class_<Class, Parent, std::shared_ptr<Class>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<>())
		.def(py::init<std::string>());
}

template <typename TDataType>
void declare_particle_rod(py::module& m, std::string typestr) {
	using Class = PhysIKA::ParticleRod<TDataType>;
	using Parent = PhysIKA::ParticleSystem<TDataType>;
	std::string pyclass_name = std::string("ParticleRod") + typestr;
	py::class_<Class, Parent, std::shared_ptr<Class>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<>())
		.def(py::init<std::string>())
		.def("set_particles", &Class::setParticles)
		.def("set_material_stiffness", &Class::setMaterialStiffness)
		.def("add_fixed_particle", &Class::addFixedParticle)
		.def_readwrite("horizon", &Class::m_horizon)
		.def_readwrite("length", &Class::m_length)
		.def_readwrite("stiffness", &Class::m_stiffness); 
}


template <typename TDataType>
void declare_elasticity_module(py::module& m, std::string typestr) {
	using Class = PhysIKA::ElasticityModule<TDataType>;
	using Parent = PhysIKA::ConstraintModule;
	std::string pyclass_name = std::string("ElasticityModule") + typestr;
	py::class_<Class, Parent, std::shared_ptr<Class>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<>())
		.def("set_iteration_number", &Class::setIterationNumber)
		.def("in_position", &Class::inPosition, py::return_value_policy::reference)
		.def("in_velocity", &Class::inVelocity, py::return_value_policy::reference)
		.def("in_horizon", &Class::inHorizon, py::return_value_policy::reference);
}

template <typename TDataType>
void declare_hyperelasticity_module(py::module& m, std::string typestr) {
	using Class = PhysIKA::HyperelasticityModule<TDataType>;
	using Parent = PhysIKA::ElasticityModule<TDataType>;
	std::string pyclass_name = std::string("HyperelasticityModule") + typestr;

	py::class_<Class, Parent, std::shared_ptr<Class>> cls(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr());
	
	cls.def(py::init<>())
		.def("set_energy_function", &Class::setEnergyFunction);

	py::enum_<Class::EnergyType>(cls, "EnergyType")
		.value("Linear", Class::EnergyType::Linear)
		.value("Quadratic", Class::EnergyType::Quadratic)
		.export_values();
}

template <typename TDataType>
void declare_elastoplasticity_module(py::module& m, std::string typestr) {
	using Class = PhysIKA::ElastoplasticityModule<TDataType>;
	using Parent = PhysIKA::ElasticityModule<TDataType>;
	std::string pyclass_name = std::string("ElastoplasticityModule") + typestr;
	py::class_<Class, Parent, std::shared_ptr<Class>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<>())
		.def("enable_fully_reconstruction", &Class::enableFullyReconstruction)
		.def("set_cohesion", &Class::setCohesion);
}

template <typename TDataType>
void declare_granular_module(py::module& m, std::string typestr) {
	using Class = PhysIKA::GranularModule<TDataType>;
	using Parent = PhysIKA::ElastoplasticityModule<TDataType>;
	std::string pyclass_name = std::string("GranularModule") + typestr;
	py::class_<Class, Parent, std::shared_ptr<Class>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<>());
}

template <typename TDataType>
void declare_fracture_module(py::module& m, std::string typestr) {
	using Class = PhysIKA::FractureModule<TDataType>;
	using Parent = PhysIKA::ElastoplasticityModule<TDataType>;
	std::string pyclass_name = std::string("FractureModule") + typestr;
	py::class_<Class, Parent, std::shared_ptr<Class>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<>());
}


template <typename TDataType>
void declare_solid_fluid_interaction(py::module& m, std::string typestr) {
	using Class = PhysIKA::SolidFluidInteraction<TDataType>;
	using Parent = PhysIKA::Node;
	std::string pyclass_name = std::string("SolidFluidInteraction") + typestr;
	py::class_<Class, Parent, std::shared_ptr<Class>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<>())
		.def("add_particle_system", &Class::addParticleSystem)
		.def("set_interaction_distance", &Class::setInteractionDistance);
}

template <typename TDataType>
void declare_multiple_fluid_model(py::module& m, std::string typestr) {
	using Class = PhysIKA::MultipleFluidModel<TDataType>;
	using Parent = PhysIKA::NumericalModel;
	std::string pyclass_name = std::string("MultipleFluidModel") + typestr;
	py::class_<Class, Parent, std::shared_ptr<Class>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<>())
		.def_readwrite("position", &Class::m_position)
		.def_readwrite("force_density", &Class::m_forceDensity)
		.def_readwrite("velocity", &Class::m_velocity);
}

template <typename TDataType>
void declare_position_based_fluid_model(py::module& m, std::string typestr) {
	using Class = PhysIKA::PositionBasedFluidModel<TDataType>;
	using Parent = PhysIKA::NumericalModel;
	std::string pyclass_name = std::string("PositionBasedFluidModel") + typestr;
	py::class_<Class, Parent, std::shared_ptr<Class>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<>())
		.def_readwrite("position", &Class::m_position)
		.def_readwrite("force_density", &Class::m_forceDensity)
		.def_readwrite("velocity", &Class::m_velocity)
		.def("set_smoothing_length", &Class::setSmoothingLength);
}


void pybind_particle_system(py::module& m)
{

	declare_static_boundary<PhysIKA::DataType3f>(m, "3f");
	declare_particle_system<PhysIKA::DataType3f>(m, "3f");

	declare_particle_elastic_body<PhysIKA::DataType3f>(m, "3f");
	declare_particle_elastoplastic_body<PhysIKA::DataType3f>(m, "3f");
	declare_particle_fluid<PhysIKA::DataType3f>(m, "3f");
	declare_particle_rod<PhysIKA::DataType3f>(m, "3f");

	declare_particle_viscoplastic_body<PhysIKA::DataType3f>(m, "3f");

	// base elasticity module
	declare_elasticity_module<PhysIKA::DataType3f>(m, "3f");

	declare_hyperelasticity_module<PhysIKA::DataType3f>(m, "3f");

	declare_elastoplasticity_module<PhysIKA::DataType3f>(m, "3f");
	declare_granular_module<PhysIKA::DataType3f>(m, "3f"); 
	declare_fracture_module<PhysIKA::DataType3f>(m, "3f");

	declare_solid_fluid_interaction<PhysIKA::DataType3f>(m, "3f");

	declare_multiple_fluid_model<PhysIKA::DataType3f>(m, "3f");
	declare_position_based_fluid_model<PhysIKA::DataType3f>(m, "3f");
}

