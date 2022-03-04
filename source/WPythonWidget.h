#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WSignal.h>

namespace dyno
{
	class SceneGraph;
}

class WPythonWidget : public Wt::WContainerWidget
{
public:
	WPythonWidget();
	~WPythonWidget();

	void setText(const std::string& text);
	void execute(const std::string& src);

	Wt::Signal<>& update() { return mUpdateSignal; }
	std::shared_ptr<dyno::SceneGraph> getSceneGraph() { return mScene; }

private:
	Wt::Signal<>						mUpdateSignal;
	std::shared_ptr<dyno::SceneGraph>	mScene;
	Wt::WText*							mCodeEditor;

};