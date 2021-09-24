CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

project(peri-cloud LANGUAGES C CXX CUDA)

#set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

#find_package(PhysIKA REQUIRED HINTS dependency)
find_package(peridyno)

# external include directory
set(EXTERNAL_DEPS ${CMAKE_SOURCE_DIR}/external)
include_directories(${EXTERNAL_DEPS})

# copy data
file(COPY "data" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "docroot" DESTINATION ${CMAKE_BINARY_DIR})


# for python binding
add_subdirectory(${CMAKE_SOURCE_DIR}/external/pybind11 pybind11)

# add sources
add_subdirectory(source)

install(TARGETS WtGUI
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

