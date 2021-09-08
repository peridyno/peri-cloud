#include "PyFramework.h"

#include "Node.h"
#include "Module/VisualModule.h"
#include "Module/ConstraintModule.h"
//#include "Framework/Framework/SceneGraph.h"
#include "Log.h"
#include "Framework/Framework/FieldArray.h"
#include "Framework/Framework/FieldVar.h"
#include "Framework/Framework/NumericalModel.h"

//using Node = PhysIKA::Node;
//using SceneGraph = PhysIKA::SceneGraph;
//using VisualModule = PhysIKA::VisualModule;
//using Log = PhysIKA::Log;

#include "Framework/Framework/NodeIterator.h"

// local patch for SceneGraph to access constructor
namespace PhysIKA {


	class SceneGraph : public Base
	{
	public:
		typedef NodeIterator Iterator;

		~SceneGraph() {};

		void setRootNode(std::shared_ptr<Node> root) { m_root = root; }
		std::shared_ptr<Node> getRootNode() { return m_root; }

		virtual bool initialize();
		bool isInitialized() { return m_initialized; }
		void invalid();

		virtual void draw();
		virtual void advance(float dt);
		virtual void takeOneFrame();
		virtual void run();

		void reset();

		virtual bool load(std::string name);

		virtual void invoke(unsigned char type, unsigned char key, int x, int y) {};

		template<class TNode, class ...Args>
		std::shared_ptr<TNode> createNewScene(Args&& ... args)
		{
			std::shared_ptr<TNode> root = TypeInfo::New<TNode>(std::forward<Args>(args)...);
			m_root = root;
			return root;
		}

	public:
		static SceneGraph& getInstance();

		inline void setTotalTime(float t) { m_maxTime = t; }
		inline float getTotalTime() { return m_maxTime; }

		inline void setFrameRate(float frameRate) { m_frameRate = frameRate; }
		inline float getFrameRate() { return m_frameRate; }
		inline float getTimeCostPerFrame() { return m_frameCost; }
		inline float getFrameInterval() { return 1.0f / m_frameRate; }
		inline int getFrameNumber() { return m_frameNumber; }

		bool isIntervalAdaptive();
		void setAdaptiveInterval(bool adaptive);

		void setGravity(Vector3f g);
		Vector3f getGravity();

		Vector3f getLowerBound();
		Vector3f getUpperBound();

		void setLowerBound(Vector3f lowerBound);
		void setUpperBound(Vector3f upperBound);

		inline Iterator begin() { return NodeIterator(m_root); }
		inline Iterator end() { return NodeIterator(nullptr); }

	public:
		SceneGraph()
			: m_elapsedTime(0)
			, m_maxTime(0)
			, m_frameRate(25)
			, m_frameNumber(0)
			, m_frameCost(0)
			, m_initialized(false)
			, m_lowerBound(0, 0, 0)
			, m_upperBound(1, 1, 1)
		{
			m_gravity = Vector3f(0.0f, -9.8f, 0.0f);
		};

		/**
		* To avoid erroneous operations
		*/
		SceneGraph(const SceneGraph&) {};
		SceneGraph& operator=(const SceneGraph&) {};

	private:
		bool m_initialized;
		bool m_advative_interval = true;

		float m_elapsedTime;
		float m_maxTime;
		float m_frameRate;
		float m_frameCost;

		int m_frameNumber;

		Vector3f m_gravity;

		Vector3f m_lowerBound;
		Vector3f m_upperBound;

	private:
		std::shared_ptr<Node> m_root = nullptr;
	};

}

using namespace PhysIKA;


template<class TNode, class ...Args>
std::shared_ptr<TNode> create_root(SceneGraph& scene, Args&& ... args) {
	return scene.createNewScene<TNode>(std::forward<Args>(args)...);
}

void pybind_log(py::module& m)
{
	py::class_<Log>(m, "Log")
		.def(py::init<>())
		.def_static("set_output", &Log::setOutput)
		.def_static("get_output", &Log::getOutput)
		.def_static("send_message", &Log::sendMessage)
		.def_static("set_level", &Log::setLevel);
}

void pybind_framework(py::module& m)
{
	pybind_log(m);

	py::class_<Node, std::shared_ptr<Node>>(m, "Node")
		.def(py::init<>())
		.def("add_child", &Node::addChild)
		.def("set_name", &Node::setName)
		.def("is_active", &Node::isActive)
		.def("set_Dt", &Node::setDt)
		.def("set_mass", &Node::setMass)
		.def("set_active", &Node::setActive)
		.def("set_visible", &Node::setVisible)
		.def("set_numerical_model", &Node::setNumericalModel)
		.def("add_visual_module", (void (Node::*)(std::shared_ptr<VisualModule>)) &Node::addVisualModule);

	py::class_<VisualModule, std::shared_ptr<VisualModule>>(m, "VisualModule")
		.def(py::init<>());

	py::class_<NumericalModel, std::shared_ptr<NumericalModel>>(m, "NumericalModel")
		.def(py::init<>());

	py::class_<ConstraintModule, std::shared_ptr<ConstraintModule>>(m, "ConstraintModule")
		.def(py::init<>());

	py::class_<SceneGraph, std::shared_ptr<SceneGraph>>(m, "SceneGraph")
		.def(py::init<>())
		//.def_static("get_instance", &SceneGraph::getInstance, py::return_value_policy::reference, "Return an instance")
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
		.def("set_lower_bound", &SceneGraph::setLowerBound)
		.def("set_upper_bound", &SceneGraph::setUpperBound)
		.def("get_upper_bound", &SceneGraph::getUpperBound);

	// ArrayField
	py::class_<DeviceArrayField<Vector3f>>(m, "DeviceArrayField3f")
		.def(py::init<>())
		.def("connect", &DeviceArrayField<Vector3f>::connect);

	py::class_<VarField<float>>(m, "VarField1f")
		.def(py::init<>())
		.def("set_value", &VarField<float>::setValue)
		.def("connect", &VarField<float>::connect);

}

