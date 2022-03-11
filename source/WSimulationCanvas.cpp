#include "WSimulationCanvas.h"
#include "ImageEncoder.h"

#include <Wt/WApplication.h>
#include <Wt/WMemoryResource.h>
#include <Wt/WImage.h>
#include <SceneGraph.h>

#include <Module/CalculateNorm.h>
#include <ColorMapping.h>

#include <GLRenderEngine.h>
#include <GLRenderTarget.h>
#include <GLPointVisualModule.h>
#include <GLSurfaceVisualModule.h>

#include <Camera.h>

#include <GLFW/glfw3.h>

using namespace dyno;

WSimulationCanvas::WSimulationCanvas()
{
	this->setLayoutSizeAware(true);
	this->setStyleClass("remote-framebuffer");
	this->resize("100%", "100%");

	this->mouseWentUp().preventDefaultAction(true);
	this->mouseWentDown().preventDefaultAction(true);
	this->mouseDragged().preventDefaultAction(true);
	this->touchStarted().preventDefaultAction(true);
	this->touchMoved().preventDefaultAction(true);
	this->touchEnded().preventDefaultAction(true);

	this->mouseWentDown().connect(this, &WSimulationCanvas::onMousePressed);
	this->mouseWheel().connect(this, &WSimulationCanvas::onMouseWheeled);
	this->mouseDragged().connect(this, &WSimulationCanvas::onMouseDrag);
	this->mouseWentUp().connect(this, &WSimulationCanvas::onMouseReleased);

	this->setAttributeValue("oncontextmenu", "return false;");

	mApp = Wt::WApplication::instance();
	
	mImage = this->addNew<Wt::WImage>();
	mImage->resize("100%", "100%");

	mImage->setJavaScriptMember("currURL", "null");
	mImage->setJavaScriptMember("nextURL", "null");
	mImage->setJavaScriptMember("onload",
		"function() {"
		"this.currURL = this.nextURL;"
		"this.nextURL = null;"
		"if (this.currURL != null) {"
		"this.src = this.currURL;"
		"}"
		"}.bind(" + mImage->jsRef() + ")");
	mImage->setJavaScriptMember("onerror",
		"function() {"
		"this.currURL = this.nextURL;"
		"this.nextURL = null;"
		"if (this.currURL != null) {"
		"this.src = this.currURL;"
		"}"
		"}.bind(" + mImage->jsRef() + ")");
	mImage->setJavaScriptMember("update",
		"function(url) { "
		"if (this.currURL == null) {"
		"this.currURL = url;"
		"this.src = this.currURL;"
		"} else {"						// still loading
		"this.nextURL = url;"
		"}"
		"}.bind(" + mImage->jsRef() + ")");


	mJpegEncoder = std::make_unique<ImageEncoderNV>();
	mJpegEncoder->SetQuality(80);

	mJpegResource = std::make_unique<Wt::WMemoryResource>("image/jpeg");

	// create an empty scene
	mScene = std::make_shared<dyno::SceneGraph>();

	mRenderEngine = new dyno::GLRenderEngine;

	mRenderTarget = new dyno::GLRenderTarget;
	mRenderTarget->mColorTex.format = GL_RGB;
	mRenderTarget->mColorTex.internalFormat = GL_RGB;	// SRGB8
	mRenderTarget->mColorTex.type = GL_UNSIGNED_BYTE;

	// initialize OpenGL context and RenderEngine
	this->initializeGL();
}

WSimulationCanvas::~WSimulationCanvas()
{
	makeCurrent();
	delete mRenderTarget;
	delete mRenderEngine;

	glfwDestroyWindow(mContext);
}

void WSimulationCanvas::initializeGL()
{
	// initialize render engine and target
	glfwInit();
	// Set all the required options for GLFW    
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	mContext = glfwCreateWindow(640, 480, "", NULL, NULL);
	if (!mContext)
	{
		Wt::log("error") << "Failed to create OpenGL context!";
		exit(-1);
	}

	makeCurrent();

	mRenderEngine->initialize(640, 480);
	mRenderTarget->initialize();

	// initialize size
	mImageData.resize(640 * 480 * 3);

	doneCurrent();

}

void WSimulationCanvas::makeCurrent()
{
	if(glfwGetCurrentContext() != mContext)
		glfwMakeContextCurrent(mContext);
}

void WSimulationCanvas::doneCurrent()
{
	glfwMakeContextCurrent(NULL);
}

void WSimulationCanvas::layoutSizeChanged(int width, int height)
{
	mImageData.resize(width * height * 3);

	this->makeCurrent();
	mRenderEngine->resize(width, height);
	mRenderTarget->resize(width, height);
	this->doneCurrent();

	WContainerWidget::layoutSizeChanged(width, height); 
	scheduleRender();
}

void WSimulationCanvas::onMousePressed(const Wt::WMouseEvent& evt)
{
	Wt::Coordinates coord = evt.widget();
	mRenderEngine->camera()->registerPoint(coord.x, coord.y);
}

void WSimulationCanvas::onMouseDrag(const Wt::WMouseEvent& evt)
{
	Wt::Coordinates coord = evt.widget();

	if (evt.button() == Wt::MouseButton::Left) {
		mRenderEngine->camera()->rotateToPoint(coord.x, coord.y);
	}
	else if (evt.button() == Wt::MouseButton::Middle) {
		mRenderEngine->camera()->translateToPoint(coord.x, coord.y);
	}
	scheduleRender();
}

void WSimulationCanvas::onMouseReleased(const Wt::WMouseEvent& evt)
{

}

void WSimulationCanvas::onMouseWheeled(const Wt::WMouseEvent& evt)
{
	mRenderEngine->camera()->zoom(-1.0*evt.wheelDelta());
	scheduleRender();
}

void WSimulationCanvas::render(Wt::WFlags<Wt::RenderFlag> flags)
{
	update();
}

void WSimulationCanvas::update()
{
	// do render
	{
		this->makeCurrent();
		mRenderTarget->bind();

		if (mScene)
		{
			mScene->updateGraphicsContext();
		}
		mRenderEngine->draw(mScene.get());

		// dump framebuffer
		mRenderTarget->mColorTex.dump(mImageData.data());
		this->doneCurrent();
	}

	// encode image
	{
		mJpegEncoder->Encode(mImageData.data(),
			mRenderTarget->width, mRenderTarget->height, 0,
			mJpegBuffer);

		Wt::log("info") << mRenderTarget->width << " x " << mRenderTarget->height
			<< ", JPG size: " << mJpegBuffer.size() / 1024 << " kb";
	}

	// update UI
	{
		mJpegResource->setData(mJpegBuffer);
		const std::string url = mJpegResource->generateUrl();
		mImage->callJavaScriptMember("update", WWebWidget::jsStringLiteral(url));
	}
}

void WSimulationCanvas::setScene(std::shared_ptr<dyno::SceneGraph> scene)
{
	this->mScene = scene;

	// TODO: move to somewhere else!
	if (this->mScene)
	{
		makeCurrent();
		this->mScene->initialize();
		doneCurrent();

		scheduleRender();
	}

}

dyno::RenderParams* WSimulationCanvas::getRenderParams()
{
	return mRenderEngine->renderParams();
};
