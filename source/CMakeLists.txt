set(target WtGUI)

file(GLOB_RECURSE SOURCES *.cpp *.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_executable(${target} ${SOURCES})

# link dyno
target_link_libraries(${target} PRIVATE ParticleSystem Peridynamics)
# link dyno Rendering2
target_link_libraries(${target} PRIVATE GLRenderEngine)

# Wt
find_package(wt REQUIRED HINTS ${CMAKE_SOURCE_DIR}/dependency)
target_link_libraries(${target} PRIVATE Wt::Wt Wt::HTTP)

# for python scripting
target_link_libraries(${target} PRIVATE pybind11::embed)

#add_dependencies(${target} PyPhysIKA)

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