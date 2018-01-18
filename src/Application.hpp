#pragma once

/*
 * File:   Application.hpp
 * Author: Jozef Kobrtek
 * Date:   24th August 2016
 */

//STL includes
#include <vector>
#include <iostream>
#include <memory>

//OpenGL stuff
#include <SDL/SDL.h>
#undef main
#include <GL/glew.h>

//Project includes
#include "HighResolutionTimer.hpp"
#include "SceneLoader.hpp"
#include "OrbitalCamera.hpp"
#include "FreelookCamera.hpp"
#include "ShaderCompiler.hpp"
#include "CameraPath.h"

#include "HSRenderer.hpp"

//Project defines
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

//If commented out, freelook camera is used instead
#define USE_ORBITAL_CAMERA

class Application
{
public:
    Application();

    //Main application function
    bool Run(int argc, char** argv);

    //Initializes window and OpenGL
    //Calls initGL function
    bool Init();

    //Loads scene from file
    //Stores all data in "scene" object
    //Scene = all the meshes from single file
    bool addModelFileToScene(const char* path, std::shared_ptr<Scene> scene, const glm::mat4& transform = glm::mat4(1));

	bool loadSceneFiles(int numModels, char** paths);

    //Setup camera for scene
    void setupCamera(OrbitalCamera& orbitalCamera);
    void setupCamera(FreelookCamera& freeCamera);

    //Clears all resources used by the application
    //Also calls clearGL
    void Clear();
    
    //Initializes SDL, GLEW
    bool initWindow();
    bool initGlew();

    //Destroys window
    void clearSDLWindow();

private:

	void _splitPathToFilenameAndDirectory(const std::string& path, std::string& directoryPath, std::string& fileName);

    //SDL window and OpenGL context
    SDL_Window*          _window;
    SDL_GLContext        _glContext;

    //Timer
    HighResolutionTimer  _timer;

    //Camera
#ifdef USE_ORBITAL_CAMERA
    OrbitalCamera        _camera;
#else
    FreelookCamera       _camera;
#endif
    //Scene data
    std::shared_ptr<Scene> _scene;

	//Student renderer
	HierarchicalSilhouetteRenderer _renderer;
};