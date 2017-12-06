cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME "ShadowVolumesRenderer")

project(${PROJECT_NAME})

set(PROJECT_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(PROJECT_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(PROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(SDL2 REQUIRED)
find_package(assimp REQUIRED)
find_package(DevIL REQUIRED)
find_package(OpenGL REQUIRED)


set(SRC_FILES
	${PROJECT_SRC_DIR}/AABB.cpp
	${PROJECT_SRC_DIR}/Application.cpp
	${PROJECT_SRC_DIR}/CameraPath.cpp
    ${PROJECT_SRC_DIR}/EdgeExtractor.cpp
	${PROJECT_SRC_DIR}/EdgeVisualizer.cpp
	${PROJECT_SRC_DIR}/FreelookCamera.cpp
    ${PROJECT_SRC_DIR}/GLProgram.cpp
	${PROJECT_SRC_DIR}/HighResolutionTimer.cpp
	${PROJECT_SRC_DIR}/HSRenderer.cpp
	${PROJECT_SRC_DIR}/main.cpp
	${PROJECT_SRC_DIR}/ModelLoader.cpp
	${PROJECT_SRC_DIR}/MultiBitArray.cpp
    ${PROJECT_SRC_DIR}/Octree.cpp
    ${PROJECT_SRC_DIR}/OGLScene.cpp
	${PROJECT_SRC_DIR}/OrbitalCamera.cpp
	${PROJECT_SRC_DIR}/SceneLoader.cpp
	${PROJECT_SRC_DIR}/ShaderCompiler.cpp
	${PROJECT_SRC_DIR}/TextureLoader.cpp
	${PROJECT_SRC_DIR}/VoxelSpace.cpp
)

set(HEADER_FILES
	${PROJECT_SRC_DIR}/AABB.hpp
	${PROJECT_SRC_DIR}/Application.hpp
	${PROJECT_SRC_DIR}/BitOperations.h
	${PROJECT_SRC_DIR}/CameraPath.h
	${PROJECT_SRC_DIR}/CommonTypes.hpp
    ${PROJECT_SRC_DIR}/EdgeExtractor.hpp
	${PROJECT_SRC_DIR}/EdgeVisualizer.hpp
	${PROJECT_SRC_DIR}/FreelookCamera.hpp
    ${PROJECT_SRC_DIR}/GLProgram.hpp
	${PROJECT_SRC_DIR}/HighResolutionTimer.hpp
	${PROJECT_SRC_DIR}/HSRenderer.hpp
	${PROJECT_SRC_DIR}/ModelLoader.hpp
	${PROJECT_SRC_DIR}/MultiBitArray.hpp
    ${PROJECT_SRC_DIR}/Octree.hpp
    ${PROJECT_SRC_DIR}/OGLScene.hpp
	${PROJECT_SRC_DIR}/OrbitalCamera.hpp
	${PROJECT_SRC_DIR}/Scene.hpp
	${PROJECT_SRC_DIR}/SceneLoader.hpp
	${PROJECT_SRC_DIR}/ShaderCompiler.hpp
	${PROJECT_SRC_DIR}/TextureLoader.hpp
	${PROJECT_SRC_DIR}/VoxelSpace.hpp
)

include_directories()

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BIN_DIR}")

# Set binary output to /bin
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BIN_DIR} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BIN_DIR} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BIN_DIR} )
# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BIN_DIR} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BIN_DIR} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BIN_DIR} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})

include_directories(${PROJECT_NAME} PUBLIC 
	${GLEW_INCLUDE_DIR} 
	${SDL2_INCLUDE_DIR}
	${IL_INCLUDE_DIR}
	${ASSIMP_INCLUDE_DIR}
	${GLM_INCLUDE_DIRS}
	)
	
target_link_libraries(${PROJECT_NAME}
	${GLEW_LIBRARIES} 
	${SDL2_LIBRARY} 
	${IL_LIBRARIES}
	${ASSIMP_LIBRARY_RELEASE}
	${OPENGL_gl_LIBRARY}
)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BIN_DIR}")
