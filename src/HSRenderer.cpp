#include <iostream>
#include <algorithm>

#include <glm/gtc/type_ptr.hpp>

#include "HSRenderer.hpp"
#include "ShaderCompiler.hpp"

#include "MultiBitArray.hpp"
#include "EdgeExtractor.hpp"
#include "GeometryOperations.hpp"
#include "HighResolutionTimer.hpp"

HierarchicalSilhouetteRenderer::HierarchicalSilhouetteRenderer()
{

}

bool HierarchicalSilhouetteRenderer::init(std::shared_ptr<Scene> scene, unsigned int screenWidth, unsigned int screenHeight)
{
	_scene = scene;
	//_scene->lightPos = glm::vec3(0, 9.9, 0);
	_scene->lightPos = glm::vec3(0, -8, 4);
	
	_initVoxelSpace();

	_initGL(screenWidth, screenHeight);

	_oglScene.loadScene(scene);

	_allocateTriangleVector();
	_generateScenePretransformedGeometry();

	EdgeExtractor extractor;
	extractor.extractEdgesFromTriangles(_pretransformedTriangles, _edges);

	std::cout << "Scene has " << _pretransformedTriangles.size() * 3 << " triangles\n";
	std::cout << "Scene has " << _edges.size() << " edges\n";
	/*
	{
		VoxelParams params;
		params.numVoxelsX = params.numVoxelsY = params.numVoxelsZ = 30;

		_silhouetteMethod = std::make_shared<BitArrayVoxelSilhouettes>();
		_silhouetteMethod->initialize(_edges, _voxelSpace, &params);

		std::cout << "Bit array has size " << _silhouetteMethod->getAccelerationStructureSizeBytes() / 1024.0f / 1024.0f << "MB\n";
	}
	//*/

	
	{
		OctreeParams params;
		params.maxDepthLevel = 5;

		//AABB space;
		//space.setMinMaxPoints(glm::vec3(-50, -50, -50), glm::vec3(50, 50, 50));

		_silhouetteMethod = std::make_shared<OctreeSilhouettes>();
		_silhouetteMethod->initialize(_edges, _voxelSpace, &params);
		std::cout << "Octree has size " << _silhouetteMethod->getAccelerationStructureSizeBytes() / 1024.0f / 1024.0f << "MB\n";
	}
	//*/

	if (!_initSidesRenderData())
		return false;
	
	if (!_loadShaders())
		return false;
	
	std::vector<int> potentialEdges;
	std::vector<int> silhouetteEdges;

	_silhouetteMethod->getSilhouetteEdgesForLightPos(_scene->lightPos, potentialEdges, silhouetteEdges);

	std::cout << "Num potential: " << potentialEdges.size() << " num silhouette: " << silhouetteEdges.size() << std::endl;

	_generateSidesFromEdgeIndices(potentialEdges, silhouetteEdges, _sides);

	_edgeVisualizer.loadEdges(_edges);
	
	_updateSides();
	
	//_initOctree();

	return true;
}

void HierarchicalSilhouetteRenderer::_initGL(unsigned int screenWidth, unsigned int screenHeight)
{
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(1, 1, 1, 1);
}
/*
void HierarchicalSilhouetteRenderer::_initOctree()
{
	AABB space;
	space.setMinMaxPoints(glm::vec3(-50, -50, -50), glm::vec3(50, 50, 50));
	
	_octree = std::make_shared<Octree>(5, space);
	_octreeVisitor = std::make_shared<OctreeVisitor>(_octree);

	HighResolutionTimer tmr;
	tmr.reset();

	//_loadOctree();
	_loadOctree2();

	const auto buildTime = tmr.getElapsedTimeMilliseconds();

	const float sz = _octree->getOctreeSizeBytes()/1024.0f/1024.0f;

	_processOctree();

	const auto processTime = tmr.getElapsedTimeFromLastQueryMilliseconds();

	std::cout << "Time to build: " << buildTime << "ms\nTime to postprocess: " << processTime << "ms" << std::endl;
	std::cout << "Unprocessed size " << sz << "MB, processed: " << _octree->getOctreeSizeBytes() / 1024.0f / 1024.0f << "MB" << std::endl;
	//_testOctree();
}


void HierarchicalSilhouetteRenderer::_loadOctree()
{
	unsigned int i = 0;
	for (const auto edge : _edges)
	{
		_octreeVisitor->addEdge(edge, i);
		++i;
	}
}

void HierarchicalSilhouetteRenderer::_loadOctree2()
{
	_octreeVisitor->addEdges(_edges);
}

void HierarchicalSilhouetteRenderer::_processOctree()
{
	_octreeVisitor->processPotentialEdges();
	_octreeVisitor->cleanEmptyNodes();
}
*/
void HierarchicalSilhouetteRenderer::onUpdate(float timeSinceLastUpdateMs)
{
	
}

void HierarchicalSilhouetteRenderer::onKeyPressed(SDL_Keycode code)
{

}

void HierarchicalSilhouetteRenderer::onWindowRedraw(glm::mat4 cameraViewProjectionMatrix, glm::vec3 cameraPosition)
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	_drawSides(cameraViewProjectionMatrix);
	_drawEdges(cameraViewProjectionMatrix);


	//_drawScenePhong(cameraViewProjectionMatrix, cameraPosition);
	//_basicProgram.bind();
	//_basicProgram.updateUniform("vp", cameraViewProjectionMatrix);
	//glUniformMatrix4fv(_vpLocationBasic, 1, GL_FALSE, glm::value_ptr(cameraViewProjectionMatrix));
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _oglScene.getMatricesSSBO());
	//_oglScene.drawWithoutNormalsTcoords();
}

void HierarchicalSilhouetteRenderer::clear()
{
    
}

void HierarchicalSilhouetteRenderer::_generateScenePretransformedGeometry()
{
	int triangleIndex = 0;

	for (const auto mesh : _scene->meshes)
	{
		for (unsigned int i = 0; i < mesh.vertices.size(); i += 3, triangleIndex++)
		{
			_tranformVertex(mesh.vertices[i+0], mesh.modelMatrix, _pretransformedTriangles[triangleIndex].v1);
			_tranformVertex(mesh.vertices[i+1], mesh.modelMatrix, _pretransformedTriangles[triangleIndex].v2);
			_tranformVertex(mesh.vertices[i+2], mesh.modelMatrix, _pretransformedTriangles[triangleIndex].v3);
		}
	}
}

void HierarchicalSilhouetteRenderer::_tranformVertex(const glm::vec4& vertex, const glm::mat4& modelMatrix, glm::vec4& transformedVertex) const
{
	transformedVertex = modelMatrix * vertex;
}

void HierarchicalSilhouetteRenderer::_allocateTriangleVector()
{
	size_t size = 0;

	for (auto mesh : _scene->meshes)
		size += mesh.vertices.size();

	size /= 3;

	_pretransformedTriangles.resize(size);
}

void HierarchicalSilhouetteRenderer::_generateSidesFromEdgeIndices(const std::vector<int>& potentialEdges, const std::vector<int>& silhouetteEdges, std::vector<glm::vec4>& sides)
{	
	unsigned int numSilhouetteEdges = silhouetteEdges.size();
	
	std::vector<int> edges;
	edges.insert(edges.end(), silhouetteEdges.begin(), silhouetteEdges.end());
	
	for(const auto edge : silhouetteEdges)
	{
		//TODO tu opravit edge==0 moze mat len jednu orientaciu
		
		const int multiplicitySign = edge >= 0 ? 1 : -1;
		_generatePushSideFromEdge(_scene->lightPos, _edges[abs(edge)].first, multiplicitySign, sides);
		
		/*
		const int multiplicity = GeometryOps::calcEdgeMultiplicity(_edges[abs(edge)], _scene->lightPos);
		if (multiplicity != 0)
		{
			_generatePushSideFromEdge(_scene->lightPos, _edges[abs(edge)].first, multiplicity, sides);
		}
		*/
	}
	
	for (const auto edge : potentialEdges)
	{
		const int multiplicity = GeometryOps::calcEdgeMultiplicity(_edges[edge], _scene->lightPos);
		if (multiplicity != 0)
		{
			_generatePushSideFromEdge(_scene->lightPos, _edges[edge].first, multiplicity, sides);
			++numSilhouetteEdges;
			edges.push_back(edge);
		}
	}

	//std::sort(edges.begin(), edges.end());

	std::cout << "Silhouette consists of " << numSilhouetteEdges << " edges\n";
	for(const auto edge: edges)
	{
		std::cout << edge << std::endl;
	}
}

void HierarchicalSilhouetteRenderer::_generatePushSideFromEdge(const glm::vec3& lightPos, const Edge& edge, int multiplicitySign, std::vector<glm::vec4>& sides) const
{
	glm::vec4 lowInfinity = glm::vec4(edge.lowerPoint - lightPos, 0);
	glm::vec4 highInfinity = glm::vec4(edge.higherPoint - lightPos, 0);

	if (multiplicitySign > 0)
	{
		sides.push_back(lowInfinity);
		sides.push_back(glm::vec4(edge.lowerPoint, 1));
		sides.push_back(glm::vec4(edge.higherPoint, 1));

		sides.push_back(highInfinity);
		sides.push_back(lowInfinity);
		sides.push_back(glm::vec4(edge.higherPoint, 1));
	}
	else if(multiplicitySign<0)
	{
		sides.push_back(highInfinity);
		sides.push_back(glm::vec4(edge.higherPoint, 1));
		sides.push_back(glm::vec4(edge.lowerPoint, 1));

		sides.push_back(lowInfinity);
		sides.push_back(highInfinity);
		sides.push_back(glm::vec4(edge.lowerPoint, 1));
	}
}

void HierarchicalSilhouetteRenderer::_updateSides()
{
	glNamedBufferData(_sidesVBO, _sides.size() * 4 * sizeof(float), _sides.data(), GL_DYNAMIC_DRAW);
}

bool HierarchicalSilhouetteRenderer::_initSidesRenderData()
{
	glGenVertexArrays(1, &_sidesVAO);
	
	glGenBuffers(1, &_sidesVBO);
	glNamedBufferDataEXT(_sidesVBO, _edges.size() * 4 * sizeof(float), nullptr, GL_STATIC_DRAW);
	glEnableVertexArrayAttribEXT(_sidesVAO, 0);
	glVertexArrayVertexAttribOffsetEXT(_sidesVAO, _sidesVBO, 0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	return true;
}

void HierarchicalSilhouetteRenderer::_drawSides(const glm::mat4& mvp)
{
	const glm::vec3 color = glm::vec3(0, 1, 0);

	_basicProgram.bind();
	_basicProgram.updateUniform("mvp", mvp);
	_basicProgram.updateUniform("color", color);

	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glBindVertexArray(_sidesVAO);
	glDrawArrays(GL_TRIANGLES, 0, GLuint(_sides.size()));

	_basicProgram.unbind();
	glBindVertexArray(0);
	glDisable(GL_DEPTH_CLAMP);
	glDisable(GL_CULL_FACE);
}

void HierarchicalSilhouetteRenderer::_drawEdges(const glm::mat4& mvp)
{
	const glm::vec3 color = glm::vec3(1, 0, 0);

	_basicProgram.bind();
	_basicProgram.updateUniform("mvp", mvp);
	_basicProgram.updateUniform("color", color);

	_edgeVisualizer.drawEdges();
}

bool HierarchicalSilhouetteRenderer::_loadShaders()
{
	bool retval = _basicProgram.makeProgram(2, GL_VERTEX_SHADER, "shaders/basic.vs", GL_FRAGMENT_SHADER, "shaders/basic.fs");
	retval &= _sceneBasicProgram.makeProgram(2, GL_VERTEX_SHADER, "shaders/sceneBasic.vs", GL_FRAGMENT_SHADER, "shaders/sceneBasic.fs");
	retval &= _scenePhongProgram.makeProgram(2, GL_VERTEX_SHADER, "shaders/scenePhong.vs", GL_FRAGMENT_SHADER, "shaders/scenePhong.fs");

	return retval;
}

void HierarchicalSilhouetteRenderer::_drawScenePhong(const glm::mat4& vp, const glm::vec3& cameraPos)
{
	assert(glGetError()==GL_NO_ERROR);
	_scenePhongProgram.bind();

	_scenePhongProgram.updateUniform("vp", vp);
	_scenePhongProgram.updateUniform("lightPos", _scene->lightPos);
	_scenePhongProgram.updateUniform("cameraPos", cameraPos);

	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, _oglScene.getMatricesSSBO(), 0, _oglScene.getMatricesSSBOSize());

	if(_oglScene.getTexturesSSBOSize())
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, _oglScene.getTexturesSSBO(), 0, _oglScene.getTexturesSSBOSize());

	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, _oglScene.getMaterialsSSBO(), 0, _oglScene.getMaterialsSSBOSize());

	_oglScene.drawWithNormalsTcoords();

	_scenePhongProgram.unbind();

	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 0, 0, 0);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, 0, 0, 0);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, 0, 0, 0);

	assert(glGetError() == GL_NO_ERROR);
}

void HierarchicalSilhouetteRenderer::_initVoxelSpace()
{
	_voxelSpace.setMinMaxPoints(glm::vec3(-10, -10, -10), glm::vec3(10, 10, 10));
}

void HierarchicalSilhouetteRenderer::_testOctree()
{
	auto a = _octree->getNodeParent(0);
	a = _octree->getNodeParent(70);

	_octree->splitNode(0);

	_octree->splitNode(1);
	_octree->splitNode(2);
	_octree->splitNode(3);
	_octree->splitNode(4);
	_octree->splitNode(5);
	_octree->splitNode(6);
	_octree->splitNode(7);
	_octree->splitNode(8);

	//Should trigger assertion
	//_octree->splitNode(1000);

	a = _octree->getChildrenStartingId(0);
	a = _octree->getChildrenStartingId(15);
	a = _octree->getChildrenStartingId(7);
	a = _octree->getChildrenStartingId(40);

	a = _octree->getNodeRecursionLevel(22);

	a = _octree->getLowestLevelCellIndexFromPointInSpace(glm::vec3(1000, 0, 0));
	a = _octree->getLowestLevelCellIndexFromPointInSpace(glm::vec3(0, 0, 0));
	AABB bb = _octree->getNodeVolume(a);

	a = _octree->getLowestLevelCellIndexFromPointInSpace(glm::vec3(5, 8, 2));
	bb = _octree->getNodeVolume(a);
}
