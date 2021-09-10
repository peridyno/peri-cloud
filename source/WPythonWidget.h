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
	dyno::SceneGraph* getSceneGraph() { return mSceneGraph; }

private:
	Wt::Signal<>						mUpdateSignal;
	dyno::SceneGraph*					mSceneGraph;
	Wt::WText*							mCodeEditor;

};