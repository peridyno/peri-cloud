#include "PyRendering.h"

#include <PhysIKA/Rendering2/module/FluidRender.h>
#include <PhysIKA/Rendering2/module/PointRender.h>
#include <PhysIKA/Rendering2/module/SurfaceRender.h>
#include <PhysIKA/Rendering2/module/HeightFieldRender.h>

#include <glad/glad.h>

using namespace PhysIKA;

/**
    Wrap the visual module to make sure the GLAD is initialzied.
*/
static int glInitialized = 0;

#define WRAP_VISUAL_MODULE(T) \
class T##Wrap : public T {\
protected:\
bool initializeGL() override{\
    if(glInitialized==0) glInitialized=gladLoadGL();\
    if(glInitialized) return T::initializeGL();\
    return false; }\
};

WRAP_VISUAL_MODULE(FluidRenderer)
WRAP_VISUAL_MODULE(PointRenderer)
WRAP_VISUAL_MODULE(SurfaceRenderer)
WRAP_VISUAL_MODULE(HeightFieldRender)

void pybind_rendering(py::module& m)
{
    py::class_<GLVisualModule, VisualModule, std::shared_ptr<GLVisualModule>>(m, "GLVisualModule")
        .def("set_color", &GLVisualModule::setColor)
        .def("set_metallic", &GLVisualModule::setMetallic)
        .def("set_roughness", &GLVisualModule::setRoughness)
        .def("set_alpha", &GLVisualModule::setAlpha)
        .def("set_colormap_mode", &GLVisualModule::setColorMapMode)
        .def("set_colormap_range", &GLVisualModule::setColorMapRange)
        .def("set_shadow_mode", &GLVisualModule::setShadowMode)
        .def("is_transparent", &GLVisualModule::isTransparent);

    py::class_<FluidRendererWrap, GLVisualModule, std::shared_ptr<FluidRendererWrap>>
        (m, "FluidRenderer", py::buffer_protocol(), py::dynamic_attr())
        .def(py::init<>())
        .def("set_point_size", &FluidRendererWrap::setPointSize)
        .def("get_point_size", &FluidRendererWrap::getPointSize);

    py::class_<PointRendererWrap, GLVisualModule, std::shared_ptr<PointRendererWrap>>
        (m, "PointRenderer", py::buffer_protocol(), py::dynamic_attr())
        .def(py::init<>())
        .def("set_point_size", &PointRendererWrap::setPointSize)
        .def("get_point_size", &PointRendererWrap::getPointSize);

    py::class_<SurfaceRendererWrap, GLVisualModule, std::shared_ptr<SurfaceRendererWrap>>
        (m, "SurfaceRenderer", py::buffer_protocol(), py::dynamic_attr())
        .def(py::init<>());

    py::class_<HeightFieldRenderWrap, GLVisualModule, std::shared_ptr<HeightFieldRenderWrap>>
        (m, "HeightFieldRender", py::buffer_protocol(), py::dynamic_attr())
        .def(py::init<>());
}