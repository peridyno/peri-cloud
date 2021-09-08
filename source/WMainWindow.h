#pragma once

#include <Wt/WContainerWidget.h>

namespace dyno
{
	class SceneGraph;
};

class WNodeDataModel;
class WModuleDataModel;
class WSimulationCanvas;
class WMainWindow : public Wt::WContainerWidget
{
public:
	WMainWindow();
	~WMainWindow();		
	
private:
	void initMenu(Wt::WMenu*);
	void initLeftPanel(Wt::WContainerWidget*);

	void start();
	void stop();
	void step();

	void setSceneGraph(std::shared_ptr<dyno::SceneGraph> scene);

private:

	WSimulationCanvas*	mSceneCanvas;

	// data models
	std::shared_ptr<WNodeDataModel>		mNodeDataModel;
	std::shared_ptr<WModuleDataModel>	mModuleDataModel;

	bool				bRunFlag;

	std::shared_ptr<dyno::SceneGraph> mSceneGraph;
};


