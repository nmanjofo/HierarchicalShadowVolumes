#pragma once

#include <map>

#include <SDL/SDL.h>
#include <GL/glew.h>

#include "Scene.hpp"
#include "CommonTypes.hpp"
#include "MultiBitArray.hpp"
#include "EdgeVisualizer.hpp"
#include "OGLScene.hpp"
#include "GLProgram.hpp"
#include "Octree.hpp"

class HierarchicalSilhouetteRenderer
{
public:
	HierarchicalSilhouetteRenderer();

	bool init(std::shared_ptr<Scene> scene, unsigned int screenWidth, unsigned int screenHeight);
	void onUpdate(float timeSinceLastUpdateMs);
	void onWindowRedraw(glm::mat4 cameraViewProjectionMatrix, glm::vec3 cameraPosition);
	void onKeyPressed(SDL_Keycode code);
	void clear();

private:
	std::shared_ptr<Scene> _scene;

	std::map<Edge, std::vector<glm::vec4> > _edges;
	std::vector<MultiBitArray> _edgeBitmasks;
	std::vector<Triangle> _pretransformedTriangles;

	std::vector<glm::vec4> _sides;

	void _initVoxelization();

	//Edge generation
	void _generateScenePretransformedGeometry();
	void _tranformVertex(const glm::vec4& vertex, const glm::mat4& modelMatrix, glm::vec4& transformedVertex) const;
	void _allocateTriangleVector();
	
	bool _initSidesRenderData();

	//Testing edges against voxels
	void _generatePerEdgeVoxelInfo(const VoxelizedSpace& lightSpace);
	void _buildTrianglePlane(const Edge& edge, const glm::vec4& oppositeVertex, Plane& plane);

	//Sides generator
	void _generateSidesFromVoxelIndex(unsigned int voxelLinearIndex, std::vector<glm::vec4>& sides);
	int  _calcEdgeMultiplicity(const std::pair<Edge, std::vector<glm::vec4> >& edge, const glm::vec3& lightPos) const;
	void _generatePushSideFromEdge(const glm::vec3& lightPos, const Edge& edge, int multiplicitySign, std::vector<glm::vec4>& sides) const;

	//Shadow volume rendering
	void _updateSides();

	void _drawSides(const glm::mat4& mvp);
	void _drawEdges(const glm::mat4& mvp);

	void _drawLightCap();
	void _drawDarkCap();

	void _drawScenePhong(const glm::mat4& vp, const glm::vec3& cameraPos);

	//OpenGL
	void _initGL(unsigned int screenWidth, unsigned int screenHeight);
	bool _loadShaders();
	bool _makeProgram(const char* vsPath, const char* fsPath, GLuint& program);

	void _testOctree();

	GLuint _sidesVBO;
	GLuint _sidesVAO;

	GLProgram _basicProgram;
	GLProgram _sceneBasicProgram;
	GLProgram _scenePhongProgram;

	EdgeVisualizer _edgeVisualizer;

	OGLScene	   _oglScene;

	std::shared_ptr<Octree>	_octree;
};
