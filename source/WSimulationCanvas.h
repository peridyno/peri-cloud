#pragma once

#include <Wt/WContainerWidget.h>

#include <memory>

#include <Rendering/Engine/OpenGL/GraphicsObject/Framebuffer.h>
#include <Rendering/Engine/OpenGL/GraphicsObject/Texture.h>


namespace dyno
{
	class Camera;
	class SceneGraph; 
	class GLRenderEngine;

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
	Wt::WImage*				mImage;
	Wt::WApplication*		mApp;

	GLFWwindow*				mContext;
	dyno::GLRenderEngine*	mRenderEngine;
	dyno::RenderParams*		mRenderParams;
	
	std::vector<unsigned char> mImageData;					// raw image	
	std::vector<unsigned char> mJpegBuffer;					// jpeg data	
	std::unique_ptr<ImageEncoder> mJpegEncoder;				// jpeg encoder	
	std::unique_ptr<Wt::WMemoryResource> mJpegResource;		// Wt resource for jpeg image

	std::shared_ptr<dyno::SceneGraph> mScene;	
	std::shared_ptr<dyno::Camera>	  mCamera;

	// internal framebuffer
	dyno::Framebuffer mFramebuffer;
	dyno::Texture2D	mFrameColor;
};