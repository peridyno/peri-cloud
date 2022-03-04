#pragma once

#include <Wt/WContainerWidget.h>

#include <memory>

namespace dyno
{
	class SceneGraph; 
	class GLRenderEngine;
	class GLRenderTarget;

	struct RenderParams;
};

struct GLFWwindow;
class ImageEncoder;

class WSimulationCanvas : public Wt::WContainerWidget
{
public:
	WSimulationCanvas();
	~WSimulationCanvas();

	void setScene(std::shared_ptr<dyno::SceneGraph> scene);

	dyno::RenderParams* getRenderParams();
	
	void update();

protected:
	void initializeGL();
	void makeCurrent();
	void doneCurrent();

	void render(Wt::WFlags<Wt::RenderFlag> flags) override;
	void layoutSizeChanged(int width, int height) override;

private:
	void onMousePressed(const Wt::WMouseEvent& evt);
	void onMouseDrag(const Wt::WMouseEvent& evt);
	void onMouseReleased(const Wt::WMouseEvent& evt);
	void onMouseWheeled(const Wt::WMouseEvent& evt);

private:
	Wt::WImage* mImage;
	Wt::WApplication* mApp;

	GLFWwindow* mContext;
	dyno::GLRenderEngine* mRenderEngine;
	dyno::GLRenderTarget* mRenderTarget;

	// raw image
	std::vector<unsigned char> mImageData;

	// jpeg encoder
	std::unique_ptr<ImageEncoder> mJpegEncoder;
	// jpeg data
	std::vector<unsigned char> mJpegBuffer;
	// Wt resource for jpeg image
	std::unique_ptr<Wt::WMemoryResource> mJpegResource;

	// scene data
	std::shared_ptr<dyno::SceneGraph> mScene;

};