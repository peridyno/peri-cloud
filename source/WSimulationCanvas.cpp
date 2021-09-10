#include "WSimulationCanvas.h"

#include <Wt/WMatrix4x4.h>
#include <Wt/WApplication.h>
#include <SceneGraph.h>

#include <Peridynamics/ElasticBody.h>
#include <Peridynamics/ElasticityModule.h>
#include <ParticleSystem/StaticBoundary.h>

#include "ParticleSystem/StaticBoundary.h"
#include "ParticleSystem/ParticleFluid.h"

#include <Module/CalculateNorm.h>
#include <ColorMapping.h>

#include <GLRenderEngine.h>
#include <GLPointVisualModule.h>
#include <GLSurfaceVisualModule.h>


using namespace dyno;

WSimulationCanvas::WSimulationCanvas() : WGLWidget()
{
	mRenderEngine = new dyno::GLRenderEngine;

	this->setLayoutSizeAware(true);
	// use server side rendering...
	this->setRenderOptions(Wt::GLRenderOption::ServerSide);
	this->setStyleClass("remote-framebuffer"); 
	// set default size...
	this->resize(Wt::WLength("100%"), Wt::WLength("100%"));

	this->mouseWentDown().connect(this, &WSimulationCanvas::onMousePressed);
	this->mouseWheel().connect(this, &WSimulationCanvas::onMouseWheeled);
	this->mouseMoved().connect(this, &WSimulationCanvas::onMouseMoved);
	this->mouseWentUp().connect(this, &WSimulationCanvas::onMouseReleased);

	this->setAttributeValue("oncontextmenu", "return false;");
}

WSimulationCanvas::~WSimulationCanvas()
{
	delete mRenderEngine;
}

void WSimulationCanvas::initializeGL()
{
	initJavaScriptMatrix4(mClientTransform);		
	setClientSideLookAtHandler(mClientTransform, // the name of the JS matrix
		0, 0, 0,							// the center point
		0, 1, 0,							// the up direction
		0.005, 0.005);						// 'speed' factors

	// initialize render engine and target
	mRenderEngine->initialize(1024, 768);

	if (!mSceneGraph)
	{
		mSceneGraph = &(dyno::SceneGraph::getInstance());

	}
	if (0) {

		auto root = mSceneGraph->createNewScene<StaticBoundary<DataType3f>>();	
		auto bunny = std::make_shared<ElasticBody<DataType3f>>();
		auto render = std::make_shared<GLSurfaceVisualModule>();		

		bunny->loadParticles("data/bunny_points.obj");
		bunny->loadSurface("data/bunny_mesh.obj");
		bunny->translate(Vec3f(0.5f, 0.5f, 0.5f));
				
		render->setColor(Vec3f(1, 1, 0));
		bunny->getSurfaceNode()->currentTopology()->connect(render->inTriangleSet());
		bunny->getSurfaceNode()->graphicsPipeline()->pushModule(render);

		root->addParticleSystem(bunny);

		//auto root = std::make_shared<dyno::StaticBoundary<DataType3f>>();
		////root->loadCube(Vec3f(-0.5, 0, -0.5), Vec3f(1.5, 2, 1.5), 0.02, true);
		//root->loadSDF("data/bowl.sdf", false);

		//mSceneGraph->setRootNode(root);

		//auto fluid = std::make_shared<dyno::ParticleFluid<dyno::DataType3f>>();
		//fluid->loadParticles(dyno::Vec3f(0.5, 0.2, 0.4), dyno::Vec3f(0.7, 1.5, 0.6), 0.005);
		//fluid->setMass(100);

		//root->addParticleSystem(fluid);

		//auto calculateNorm = std::make_shared<dyno::CalculateNorm<dyno::DataType3f>>();
		//auto colorMapper = std::make_shared<dyno::ColorMapping<dyno::DataType3f>>();
		//colorMapper->varMax()->setValue(5.0f);

		//fluid->currentVelocity()->connect(calculateNorm->inVec());
		//calculateNorm->outNorm()->connect(colorMapper->inScalar());

		//fluid->graphicsPipeline()->pushModule(calculateNorm);
		//fluid->graphicsPipeline()->pushModule(colorMapper);

		//auto ptRender = std::make_shared<dyno::GLPointVisualModule>();
		//ptRender->setColor(dyno::Vec3f(1, 0, 0));
		//ptRender->setColorMapMode(dyno::GLPointVisualModule::PER_VERTEX_SHADER);
		//ptRender->setColorMapRange(0, 5);

		//fluid->currentTopology()->connect(ptRender->inPointSet());
		//colorMapper->outColor()->connect(ptRender->inColor());
		//fluid->graphicsPipeline()->pushModule(ptRender);
		
		mSceneGraph->initialize();
	}
}

void WSimulationCanvas::resizeGL(int width, int height)
{
	mRenderEngine->resize(width, height);
	repaintGL(Wt::GLClientSideRenderer::PAINT_GL);
}

void WSimulationCanvas::onMousePressed(const Wt::WMouseEvent& evt)
{
	mButtonState = WButtonState::WT_BUTTON_DOWN;
	mButtonType = evt.button();

	Wt::Coordinates coord = evt.widget();
	mRenderEngine->camera()->registerPoint(coord.x, coord.y);
}

void WSimulationCanvas::onMouseMoved(const Wt::WMouseEvent& evt)
{
	Wt::Coordinates coord = evt.widget();
	
	if (mButtonType == Wt::MouseButton::Left && mButtonState == WT_BUTTON_DOWN) {
		mRenderEngine->camera()->rotateToPoint(coord.x, coord.y);
		repaintGL(Wt::GLClientSideRenderer::PAINT_GL);
	}
	else if (mButtonType == Wt::MouseButton::Middle && mButtonState == WT_BUTTON_DOWN) {
		mRenderEngine->camera()->translateToPoint(coord.x, coord.y);
		repaintGL(Wt::GLClientSideRenderer::PAINT_GL);
	}
}

void WSimulationCanvas::onMouseReleased(const Wt::WMouseEvent& evt)
{
	mButtonState = WButtonState::WT_BUTTON_UP;
}

void WSimulationCanvas::onMouseWheeled(const Wt::WMouseEvent& evt)
{
	mRenderEngine->camera()->zoom(-1.0*evt.wheelDelta());
	repaintGL(Wt::GLClientSideRenderer::UPDATE_GL | Wt::GLClientSideRenderer::PAINT_GL);
}

void WSimulationCanvas::paintGL()
{
	mSceneGraph->updateGraphicsContext();
	mRenderEngine->draw(mSceneGraph);

	finish();
}

void WSimulationCanvas::setSceneGraph(dyno::SceneGraph* scene)
{
	this->mSceneGraph = scene;
	// reset camera and paint
	repaintGL(Wt::GLClientSideRenderer::UPDATE_GL | Wt::GLClientSideRenderer::PAINT_GL);
}

