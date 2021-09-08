#include "WPythonWidget.h"

#include <Wt/WTextArea.h>
#include <Wt/WPushButton.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WMessageBox.h>
#include <Wt/WJavaScript.h>

#include <SceneGraph.h>

// python
#include <pybind11/embed.h>
namespace py = pybind11;

WPythonWidget::WPythonWidget()
{
	this->setLayoutSizeAware(true);
	this->setOverflow(Wt::Overflow::Auto);
	this->setHeight(Wt::WLength("100%"));
	this->setMargin(0);

	auto layout = this->setLayout(std::make_unique<Wt::WVBoxLayout>());
	layout->setContentsMargins(0, 0, 0, 0);

	mCodeEditor = layout->addWidget(std::make_unique<Wt::WText>(), 1);
	mCodeEditor->setInline(false);
	mCodeEditor->setWidth(Wt::WLength("640px"));

	// ACE editor
	std::string ref = mCodeEditor->jsRef(); // is a text string that will be the element when executed in JS

	std::string command =
		ref + ".editor = ace.edit(" + ref + ");" +
		ref + ".editor.setTheme(\"ace/theme/github\");" +
		ref + ".editor.getSession().setMode(\"ace/mode/python\");";
	mCodeEditor->doJavaScript(command);

	// create signal
	auto jsignal = new Wt::JSignal<std::string>(mCodeEditor, "update");
	jsignal->connect(this, &WPythonWidget::execute);
	
	auto str = jsignal->createCall({ ref + ".editor.getValue()" });
	command = "function(object, event) {" + str + ";}";
	auto btn = layout->addWidget(std::make_unique<Wt::WPushButton>("Update"), 0);
	btn->clicked().connect(command);

	// some default code here...
	std::string source = R"====(# dyno sample
import PyPhysIKA as pk

# create body...
body= pk.ParticleElasticBody3f()
body.load_particles('data/bunny_points.obj')
# simple translate
trans1 = pk.Vector3f([0.5, 0.2, 0.5])
body.translate(trans1)
# add visual module
r1 = pk.PointRenderer()
r1.set_color(pk.Vector3f([0.4, 0.6, 0.8]))
body.add_visual_module(r1)
# create root node...
root = pk.StaticBoundary3f()
root.load_cube(pk.Vector3f([0, 0, 0]), pk.Vector3f([1, 1, 1]), 0.005, True, False)
root.add_particle_system(body)    
# create scene graph
scene = pk.SceneGraph()
scene.set_root_node(root))====";

	setText(source);
}


WPythonWidget::~WPythonWidget()
{
}

void WPythonWidget::setText(const std::string& text)
{	
	// update code editor content
	std::string ref = mCodeEditor->jsRef();
	std::string command = ref + ".editor.setValue(`" + text + "`, 1);";
	mCodeEditor->doJavaScript(command);
	//mCodeEditor->refresh();
}

void WPythonWidget::execute(const std::string& src)
{
	bool flag = true;
	py::scoped_interpreter guard{};

	try {
		auto locals = py::dict();
		py::exec(src, py::globals(), locals);

		mSceneGraph = locals["scene"].cast<std::shared_ptr<dyno::SceneGraph>>();
		mSceneGraph->initialize();
	}
	catch (const std::exception& e) {
		Wt::WMessageBox::show("Error", e.what(), Wt::StandardButton::Ok);
		flag = false;
	}

	if (flag)
	{
		mUpdateSignal.emit();
	}
}
