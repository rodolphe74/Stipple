cmake_minimum_required(VERSION 3.0)
project(Stipple)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
set(OpenGL_GL_PREFERENCE LEGACY) # Linux OpenGL
find_package(OpenGL REQUIRED)

find_package(CImg CONFIG REQUIRED)

add_executable(stipple 
		Stipple.cpp 
		ImageUtils.cpp
		jc_voronoi.h
		jc_voronoi_clip.h
		)
target_link_libraries(stipple PRIVATE sfml-system sfml-network sfml-graphics sfml-window sfml-audio ${OPENGL_LIBRARY} CImg::CImg)

add_executable(cimg_test
		CImgTest.cpp ImageUtils.cpp)
target_link_libraries(cimg_test CImg::CImg sfml-system sfml-network sfml-graphics sfml-window sfml-audio)