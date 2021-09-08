file(GLOB_RECURSE SOURCE_FILES "*.c*" "*.h*")
pybind11_add_module(PyPhysIKA ${SOURCE_FILES})

target_link_libraries(PyPhysIKA PRIVATE Rendering2)