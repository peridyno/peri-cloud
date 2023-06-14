CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

project(peri-cloud LANGUAGES C CXX CUDA)

#set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

# external include directory
include_directories(${PROJECT_SOURCE_DIR}/external)

# copy data
file(COPY "data" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "docroot" DESTINATION ${CMAKE_BINARY_DIR})

set(target PeriCloud)

# add sources
file(GLOB_RECURSE SOURCES "source/*.cpp" "source/*.h")
add_executable(${target} ${SOURCES})

# glfw3 for OpenGL context
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/glfw3 glfw3)
target_link_libraries(${target} PRIVATE glfw)

# nvJPEG
find_package(CUDAToolkit REQUIRED)
target_link_libraries(${target} PRIVATE CUDA::nvjpeg)

# Wt
find_package(wt REQUIRED)
set_target_properties(Wt::Wt PROPERTIES MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
set_target_properties(Wt::HTTP PROPERTIES MAP_IMPORTED_CONFIG_RELEASE RelWithDebInfo)
target_link_libraries(${target} PRIVATE Wt::Wt Wt::HTTP)

# link dyno
find_package(Peridyno REQUIRED)
target_link_libraries(${target} PRIVATE Topology ParticleSystem Peridynamics GLRenderEngine)

# for python scripting
# Jian: put pybind11 at end since there could be header conflict for node.h
add_subdirectory(${PROJECT_SOURCE_DIR}/external/pybind11 pybind11)
target_link_libraries(${target} PRIVATE pybind11::embed)

if(MSVC)
    set_target_properties(${target} 
		PROPERTIES 
			#COMPILE_FLAGS "/W3 /wd4251 /wd4275" # /Wp64
			VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
			#VS_DEBUGGER_COMMAND_ARGUMENTS "--docroot ${CMAKE_SOURCE_DIR}/docroot --http-listen 0.0.0.0:5000"
			)
			
	# set as startup project for visual studio
	set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${target})
endif(MSVC) 

install(TARGETS ${target}
	DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY "data" DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY "docroot"	DESTINATION ${CMAKE_INSTALL_BINDIR})

# install dependency files
install(FILES "dependency/bin/wtd.dll" "dependency/bin/wthttpd.dll"
	TYPE BIN CONFIGURATIONS Debug OPTIONAL)
install(FILES "dependency/bin/wt.dll" "dependency/bin/wthttp.dll"
	TYPE BIN CONFIGURATIONS Release OPTIONAL)
install(DIRECTORY dependency/bin DESTINATION . OPTIONAL
	FILES_MATCHING PATTERN "python*.*")

