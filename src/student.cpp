#include "Application.hpp"



//Initialize shaders, buffers etc
//Scene data is in "_scene" object
//Look into Scene.hpp for details
//Return true, if initialization was successful
bool Application::initStudentData()
{
	GLint err = glGetError();
	assert(err == GL_NO_ERROR);
    //Init geometry

    for(unsigned int i = 0; i<_scene.meshes.size(); ++i)
    {
        GLuint vao, vbo, nbo, tbo;
        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, _scene.meshes[i].vertices.size() * 4 * sizeof(float), _scene.meshes[i].vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &nbo);
        glBindBuffer(GL_ARRAY_BUFFER, nbo);
        glBufferData(GL_ARRAY_BUFFER, _scene.meshes[i].normals.size() * 3 * sizeof(float), _scene.meshes[i].normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &tbo);
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glBufferData(GL_ARRAY_BUFFER, _scene.meshes[i].texcoords.size() * 2 * sizeof(float), _scene.meshes[i].texcoords.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);

        err = glGetError();
		assert(err == GL_NO_ERROR);

        _VAOs.push_back(vao);
        _vertices.push_back(vbo);
        _normals.push_back(nbo);
        _tcoords.push_back(tbo);
    }

    //Load materials
    glGenBuffers(1, &_materialBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, _scene.materials.size() * sizeof(Material), _scene.materials.data(), GL_STATIC_READ);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    //Load textures
    std::vector<uint64_t> texHandles;

    for(unsigned int i=0; i<_scene.textures.size(); ++i)
    {
        err = glGetError();
		assert(err == GL_NO_ERROR);
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        err = glGetError();
		assert(err == GL_NO_ERROR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _scene.textures[i].width, _scene.textures[i].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _scene.textures[i].data.get());
        err = glGetError();
		assert(err == GL_NO_ERROR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_REPEAT);

        //Bindless texture
        uint64_t handle = glGetTextureHandleARB(tex);
        glMakeTextureHandleResidentARB(handle);
        texHandles.push_back(handle);
        _textures.push_back(tex);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    err = glGetError();
	assert(err == GL_NO_ERROR);

    glGenBuffers(1, &_texturesBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _texturesBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint64_t) * texHandles.size(), texHandles.data(), GL_STATIC_READ);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    //Compile shaders
    ShaderCompiler compiler;
    std::string log;

    if(compiler.compileShader(_vertexShader, "shaders/vertex.glsl", GL_VERTEX_SHADER, log) == false)
    {
        std::cerr << "Vertex shader compilation error:\n"<< log;
        return false;
    }

    if(compiler.compileShader(_fragmentShader, "shaders/fragment.glsl", GL_FRAGMENT_SHADER, log) == false)
    {
        std::cerr << "Fragment shader compilation error:\n" << log;
        return false;
    }

    if(compiler.linkProgram(_shaderProgram, log, 2, _vertexShader, _fragmentShader) == false)
    {
        std::cerr << "Program linking error:\n" << log;
        return false;
    }

    //Get uniforms from shaders
	_modelUniform = glGetUniformLocation(_shaderProgram, "modelMatrix");
    if(_modelUniform<0)
    {
        std::cerr << "Uniform modelMatrix acquire error\n";
        //return false;
    }

	_viewProjUniform = glGetUniformLocation(_shaderProgram, "viewProjectionMatrix");
	if (_viewProjUniform<0)
	{
		std::cerr << "Uniform viewProjection acquire error\n";
		//return false;
	}

	_normalUniform= glGetUniformLocation(_shaderProgram, "normalMatrix");
	if (_normalUniform<0)
	{
		std::cerr << "Uniform normalMatrix acquire error\n";
		//return false;
	}
    
    _lightPosUniform = glGetUniformLocation(_shaderProgram, "lp");
    if(_lightPosUniform<0)
    {
        std::cerr << "Uniform lp acquire error\n";
        //return false;
    }
    
    _materialUniform = glGetUniformLocation(_shaderProgram, "materialIndex");
    if(_materialUniform<0)
    {
        std::cerr << "Uniform materialIndex acquire error\n";
        //return false;
    }

    _camPosUniform = glGetUniformLocation(_shaderProgram, "camPos");
    if(_camPosUniform<0)
    {
        std::cerr << "Uniform camPos acquire error\n";
        //return false;
    }

    //Setup viewport
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClearColor(100/255.0f, 149/255.0f, 237/255.0f, 1.0f);
    glClearDepth(1.0f);

    //Misc
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_FRAMEBUFFER_SRGB);

    _lightPos = glm::vec3(0, 100, 0);
    return true;
}


//Update time-based stuff
//Argument is time delta in milliseconds
//E.g. animations, ...
void Application::onUpdate(float dt_ms)
{

}

//Here you can bind keyboard keys to a certain action
//F.e. modifiyng shader variables, colors, etc...
//To get, what key was pressed, see the last column of this table
//https://wiki.libsdl.org/SDL_Keycode
void Application::onKeyPressed(SDL_Keycode code)
{
    
}

//Main drawing function
//Acquire matrices from camera
//Draw scene
//Do not call SDL::present
void Application::onWindowRedraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(_shaderProgram);

    //Set uniforms
    glm::mat4 vp = _camera.getViewProjectionMatrix();
    glUniformMatrix4fv(_viewProjUniform, 1, GL_FALSE, glm::value_ptr(vp));

    glUniform3f(_lightPosUniform, _lightPos.x, _lightPos.y, _lightPos.z);
    glm::vec3 cp = _camera.getPosition();
    glUniform3f(_camPosUniform, cp.x, cp.y, cp.z);

	assert(glGetError() == GL_NO_ERROR);

    for(unsigned int i=0; i<_scene.meshes.size(); ++i)
	//for (unsigned int i = 0; i<1; ++i)
    {
		glBindVertexArray(_VAOs[i]);

        glUniform1ui(_materialUniform, _scene.meshes[i].materialIndex);
		glUniformMatrix4fv(_modelUniform, 1, GL_FALSE, glm::value_ptr(_scene.meshes[i].modelMatrix));
		glUniformMatrix4fv(_normalUniform, 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(_scene.meshes[i].modelMatrix))));

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _materialBuffer);
        if(_scene.materials[_scene.meshes[i].materialIndex].textureIndex >= 0)
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _texturesBuffer);

        glDrawArrays(GL_TRIANGLES, 0, _scene.meshes[i].vertices.size());
    }

	assert(glGetError() == GL_NO_ERROR);
}

//Implement this to free OpenGL resources
void Application::clearStudentData()
{
    for(unsigned int i=0; i<_VAOs.size(); ++i)
    {
        glDeleteVertexArrays(1, &_VAOs[i]);
    }
    
    glDeleteBuffers(1, &_materialBuffer);
    glDeleteBuffers(1, &_texturesBuffer);
    
    for(unsigned int i=0; i<_vertices.size(); ++i)
    {
        glDeleteBuffers(1, &_vertices[i]);
    }

    for(unsigned int i=0; i<_normals.size(); ++i)
    {
        glDeleteBuffers(1, &_normals[i]);
    }

    for(unsigned int i=0; i<_tcoords.size(); ++i)
    {
        glDeleteBuffers(1, &_tcoords[i]);
    }

    for(unsigned int i=0; i<_textures.size(); ++i)
    {
        glDeleteTextures(1, &_textures[i]);
    }
}