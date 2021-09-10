cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE SOURCES *.h *.cpp *.c)

pybind11_add_module(PyPeridyno ${SOURCES})

target_link_libraries(PyPeridyno PRIVATE Core Framework IO RigidBody ParticleSystem Peridynamics GLRenderEngine)

