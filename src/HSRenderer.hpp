#pragma once

#include <map>

#include <SDL/SDL.h>
#include <GL/glew.h>

#include "Scene.hpp"
#include "Edge.hpp"
#include "Plane.hpp"
#include "Triangle.hpp"
#include "MultiBitArray.hpp"
#include "EdgeVisualizer.hpp"
#include "OGLScene.hpp"
#include "GLProgram.hpp"
#include "Octree.hpp"
#include "OctreeVisitor.hpp"
#include "BitArrayVoxelSilhouettes.hpp"
#include "OctreeSilhouettes.hpp"

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
	void _initVoxelSpace();
	//void _initOctree();

	//Edge generation
	void _generateScenePretransformedGeometry();
	void _tranformVertex(const glm::vec4& vertex, const glm::mat4& modelMatrix, glm::vec4& transformedVertex) const;
	void _allocateTriangleVector();
	
	bool _initSidesRenderData();

	//Testing edges against voxels
	//void _generatePerEdgeVoxelInfo(const VoxelizedSpace& lightSpace);

	//Sides generator
	void _generateSidesFromEdgeIndices(const std::vector<int>& potentialEdges, const std::vector<int>& silhouetteEdges, std::vector<glm::vec4>& sides);
	void _generatePushSideFromEdge(const glm::vec3& lightPos, const Edge& edge, int multiplicitySign, std::vector<glm::vec4>& sides) const;

	//Shadow volume rendering
	void _updateSides();

	void _visualizeSides(const glm::mat4& mvp);
	void _visualizeEdges(const glm::mat4& mvp);

	void _drawLightCap();
	void _drawDarkCap();

	void _drawScenePhong(const glm::mat4& vp, const glm::vec3& cameraPos);

	//OpenGL
	void _initGL(unsigned int screenWidth, unsigned int screenHeight);
	bool _loadShaders();

	//void _loadOctree();
	//void _loadOctree2();
	//void _processOctree();
	//void _testOctree();
	

	GLuint _sidesVBO;
	GLuint _sidesVAO;

	GLProgram _basicProgram;
	GLProgram _sceneBasicProgram;
	GLProgram _scenePhongProgram;

	EdgeVisualizer _edgeVisualizer;

	OGLScene	   _oglScene;

	AABB		   _voxelSpace;

	std::shared_ptr<Scene> _scene;

	EDGE_CONTAINER_TYPE _edges;
	std::vector<Triangle> _pretransformedTriangles;

	std::vector<glm::vec4> _sides;

	std::shared_ptr<AbstractSilhouetteMethod> _silhouetteMethod;
};
