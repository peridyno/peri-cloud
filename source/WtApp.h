#pragma once

#include <Wt/WApplication.h>

#include <SceneGraph.h>

class WMainWindow;
class WtApp : public Wt::WApplication
{
public:
	WtApp(const Wt::WEnvironment& env);

private:
	WMainWindow* window;

	std::shared_ptr<dyno::SceneGraph> scene;
};