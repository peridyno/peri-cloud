#include "WMainWindow.h"
#include "WSceneDataModel.h"
#include "WSimulationCanvas.h"
#include "WSampleWidget.h"
#include "WRenderParamsWidget.h"
#include "WPythonWidget.h"

#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WBorderLayout.h>
#include <Wt/WPushButton.h>
#include <Wt/WPanel.h>
#include <Wt/WApplication.h>
#include <Wt/WMenu.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WNavigationBar.h>
#include <Wt/WTreeView.h>
#include <Wt/WTableView.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WText.h>

#include <fstream>
#include <SceneGraph.h>

WMainWindow::WMainWindow()
	: WContainerWidget(), bRunFlag(false)
{
	// disable page margin...
	setMargin(0);

	auto layout = this->setLayout(std::make_unique<Wt::WBorderLayout>());
	layout->setContentsMargins(0, 0, 0, 0);

	auto centralWidget = layout->addWidget(std::make_unique<WContainerWidget>(), Wt::LayoutPosition::Center);
	auto layout0 = centralWidget->setLayout(std::make_unique<Wt::WHBoxLayout>());
	layout0->setContentsMargins(0, 0, 0, 0);

	auto layout1 = layout0->addLayout(std::make_unique<Wt::WVBoxLayout>());
	auto layout2 = layout0->addLayout(std::make_unique<Wt::WBorderLayout>(), 1);
	auto layout3 = layout0->addLayout(std::make_unique<Wt::WVBoxLayout>(), 0);
	layout0->setResizable(0, true, 320);

	auto widget0 = layout1->addWidget(std::make_unique<Wt::WContainerWidget>());
	auto widget1 = layout3->addWidget(std::make_unique<Wt::WStackedWidget>());
	mSceneCanvas = layout2->addWidget(std::make_unique<WSimulationCanvas>(), Wt::LayoutPosition::Center);

	//create a navigation bar
	auto naviBar = layout->addWidget(std::make_unique<Wt::WNavigationBar>(), Wt::LayoutPosition::North);
	naviBar->addStyleClass("main-nav");
	naviBar->setTitle("PeriDyno", "https://github.com/peridyno/peridyno");
	naviBar->setMargin(0);

	// menu
	auto menu = naviBar->addMenu(std::make_unique<Wt::WMenu>(widget1), Wt::AlignmentFlag::Right);

	initMenu(menu);
	initLeftPanel(widget0);
}

WMainWindow::~WMainWindow()
{

}

void WMainWindow::initMenu(Wt::WMenu* menu)
{
	menu->setMargin(5, Wt::Side::Right);

	auto sampleWidget = new WSampleWidget();
	auto paramsWidget = new WRenderParamsWidget(mSceneCanvas->getRenderParams());
	auto pythonWidget = new WPythonWidget;

	menu->addItem("Samples", std::unique_ptr<WSampleWidget>(sampleWidget));
	menu->addItem("Settings", std::unique_ptr<WRenderParamsWidget>(paramsWidget));
	auto pythonItem = menu->addItem("Python", std::unique_ptr<WPythonWidget>(pythonWidget));

	paramsWidget->valueChanged().connect([=]() {
		mSceneCanvas->repaintGL(Wt::GLClientSideRenderer::PAINT_GL);
		});

	pythonWidget->update().connect([=]() {
			setSceneGraph(pythonWidget->getSceneGraph());
		});

	sampleWidget->clicked().connect([=](Sample* sample)
		{
			if (sample != NULL)
			{
				pythonItem->select();

				std::string path = sample->source();
				std::ifstream ifs(path);
				if (ifs.is_open())
				{
					std::string content((std::istreambuf_iterator<char>(ifs)),
						(std::istreambuf_iterator<char>()));
					pythonWidget->setText(content);
					pythonWidget->execute(content);
				}
			}
		});

	auto hide = menu->addItem(">>", 0);
	hide->select();
	hide->clicked().connect([=]() {
		menu->contentsStack()->setCurrentWidget(0);
	});

}


void WMainWindow::initLeftPanel(Wt::WContainerWidget* parent)
{
	// create data model
	mNodeDataModel = std::make_shared<WNodeDataModel>();
	mModuleDataModel = std::make_shared<WModuleDataModel>();

	// vertical layout
	auto layout = parent->setLayout(std::make_unique<Wt::WVBoxLayout>());
	layout->setContentsMargins(0, 0, 0, 0);
	parent->setMargin(0);

	// node tree
	auto panel0 = layout->addWidget(std::make_unique<Wt::WPanel>(), 2);
	panel0->setTitle("Node Tree");
	panel0->setCollapsible(true);	
	auto treeView = panel0->setCentralWidget(std::make_unique<Wt::WTreeView>());
	treeView->setSortingEnabled(false);
	treeView->setSelectionMode(Wt::SelectionMode::Single);
	treeView->setEditTriggers(Wt::EditTrigger::None);
	treeView->setColumnResizeEnabled(true);
	treeView->setModel(mNodeDataModel);
	treeView->setColumnWidth(0, 150);

	// module list
	auto panel1 = layout->addWidget(std::make_unique<Wt::WPanel>(), 1);
	panel1->setTitle("Module List");
	panel1->setCollapsible(true);
	auto tableView = panel1->setCentralWidget(std::make_unique<Wt::WTableView>());	treeView->setSortingEnabled(false);	
	tableView->setSortingEnabled(false); 
	tableView->setSelectionMode(Wt::SelectionMode::Single);
	tableView->setEditTriggers(Wt::EditTrigger::None);
	tableView->setModel(mModuleDataModel);

	// action for selection change
	treeView->clicked().connect([=](const Wt::WModelIndex& idx, const Wt::WMouseEvent& evt)
		{
			auto node = mNodeDataModel->getNode(idx);
			mModuleDataModel->setNode(node);
		});

	tableView->doubleClicked().connect([=](const Wt::WModelIndex& idx, const Wt::WMouseEvent& evt)
		{
			auto mod = mModuleDataModel->getModule(idx);
			if (mod->getModuleType() == "VisualModule")
			{
				Wt::log("info") << mod->getName();
			}
		});

	// simulation control
	auto panel2 = layout->addWidget(std::make_unique<Wt::WPanel>(), 0);
	panel2->setTitle("Simulation Control");
	panel2->setCollapsible(true);
	auto widget2 = panel2->setCentralWidget(std::make_unique<Wt::WContainerWidget>());
	auto layout2 = widget2->setLayout(std::make_unique<Wt::WVBoxLayout>());
	auto startButton = layout2->addWidget(std::make_unique<Wt::WPushButton>("Start"));
	auto stopButton = layout2->addWidget(std::make_unique<Wt::WPushButton>("Stop"));
	auto stepButton = layout2->addWidget(std::make_unique<Wt::WPushButton>("Step"));

	// actions
	stepButton->clicked().connect(this, &WMainWindow::step);
	startButton->clicked().connect(this, &WMainWindow::start);
	stopButton->clicked().connect(this, &WMainWindow::stop);
}

void WMainWindow::start()
{
	if (mSceneCanvas->sceneGraph())
	{
		this->bRunFlag = true;
		Wt::WApplication* app = Wt::WApplication::instance();
		while (this->bRunFlag)
		{
			step();
			app->processEvents();
		}
	}
}

void WMainWindow::stop()
{
	this->bRunFlag = false;
}

void WMainWindow::step()
{
	if (mSceneCanvas->sceneGraph())
	{
 		mSceneCanvas->sceneGraph()->takeOneFrame();
// 		mSceneCanvas->sceneGraph()->updateGraphicsContext();
		mSceneCanvas->repaintGL(Wt::GLClientSideRenderer::PAINT_GL);
	}

	std::cout << "Step!!!" << std::endl;
}

void WMainWindow::setSceneGraph(dyno::SceneGraph* scene)
{
	// try to stop the simulation
	stop();
	// setup scene graph
	mSceneGraph = scene;
	mSceneCanvas->setSceneGraph(mSceneGraph);
	mNodeDataModel->setScene(mSceneGraph);
	mModuleDataModel->setNode(0);
}
