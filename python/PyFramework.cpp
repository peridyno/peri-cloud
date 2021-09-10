#include "PyPeridyno.h"

#include <SceneGraph.h>
#include <Module/Pipeline.h>
#include <Module/GraphicsPipeline.h>
#include <Module/TopologyModule.h>

using namespace dyno;

//template<class TNode, class ...Args>
//std::shared_ptr<TNode> create_root(SceneGraph& scene, Args&& ... args) {
//	return scene.createNewScene<TNode>(std::forward<Args>(args)...);
//}

void pybind_framework(py::module& m)
{
	py::class_<Module, std::shared_ptr<Module>>(m, "Module");
	
	py::class_<Node, std::shared_ptr<Node>>(m, "Node")
		.def(py::init<>())
		.def("set_name", &Node::setName)
		.def("is_active", &Node::isActive);
		//.def("add_visual_module", [](std::shared_ptr<Node> n, std::shared_ptr<Module> m)
		//	{
		//		n->graphicsPipeline()->pushModule(m);
		//	});

	py::class_<SceneGraph>(m, "SceneGraph")
		.def_static("get_instance", &SceneGraph::getInstance, py::return_value_policy::reference, "Return an instance")
		.def("set_root_node", &SceneGraph::setRootNode)
		.def("is_initialized", &SceneGraph::isInitialized)
		.def("initialize", &SceneGraph::initialize)
		.def("set_total_time", &SceneGraph::setTotalTime)
		.def("get_total_time", &SceneGraph::getTotalTime)
		.def("set_frame_rate", &SceneGraph::setFrameRate)
		.def("get_frame_rate", &SceneGraph::getFrameRate)
		.def("get_timecost_perframe", &SceneGraph::getTimeCostPerFrame)
		.def("get_frame_interval", &SceneGraph::getFrameInterval)
		.def("get_frame_number", &SceneGraph::getFrameNumber)
		.def("set_gravity", &SceneGraph::setGravity)
		.def("get_gravity", &SceneGraph::getGravity)
		.def("get_lower_bound", &SceneGraph::getLowerBound)
		.def("set_upper_bound", &SceneGraph::setUpperBound);
}

