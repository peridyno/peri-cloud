#pragma once

#include <Wt/WGLWidget.h>

#include "WPredefs.h"

namespace dyno
{
	class SceneGraph; 
	class RenderEngine;
	class RenderTarget;
	struct RenderParams;
};

#include <memory>

#include "GLRenderEngine.h"
#include "Camera.h"

class WSimulationCanvas : public Wt::WGLWidget
{
public:
	WSimulationCanvas();
	~WSimulationCanvas();

	void setSceneGraph(std::shared_ptr<dyno::SceneGraph> scene);

 	dyno::RenderEngine* getRenderEngine() { return mRenderEngine; };
 	dyno::RenderParams* getRenderParams() { return mRenderEngine->renderParams(); };

	std::shared_ptr<dyno::SceneGraph> sceneGraph() {
		return mSceneGraph;
	}

protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int width, int height) override;

private:
	void onMousePressed(const Wt::WMouseEvent& evt);
	void onMouseMoved(const Wt::WMouseEvent& evt);
	void onMouseReleased(const Wt::WMouseEvent& evt);
	void onMouseWheeled(const Wt::WMouseEvent& evt);

private:
	// scene data
	std::shared_ptr<dyno::SceneGraph>	mSceneGraph;

	dyno::GLRenderEngine* mRenderEngine;

	// A client-side JavaScript matrix for model transform
	JavaScriptMatrix4x4		mClientTransform;

	WButtonState mButtonState = WButtonState::WT_BUTTON_UP;
	Wt::MouseButton mButtonType = Wt::MouseButton::None;
};