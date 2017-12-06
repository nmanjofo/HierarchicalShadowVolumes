#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#include "HSRenderer.hpp"
#include "ShaderCompiler.hpp"

#include "MultiBitArray.hpp"
#include "EdgeExtractor.hpp"

HierarchicalSilhouetteRenderer::HierarchicalSilhouetteRenderer()
{

}

bool HierarchicalSilhouetteRenderer::init(std::shared_ptr<Scene> scene, unsigned int screenWidth, unsigned int screenHeight)
{
	_scene = scene;
	_scene->lightPos = glm::vec3(0, 10, 0);
	
	_initVoxelization();

	_initGL(screenWidth, screenHeight);

	_oglScene.loadScene(scene);

	_allocateTriangleVector();
	_generateScenePretransformedGeometry();

	EdgeExtractor extractor;
	extractor.extractEdgesFromTriangles(_pretransformedTriangles, _edges);

	_generatePerEdgeVoxelInfo(_scene->lightSpace);

	if (!_initSidesRenderData())
		return false;
	
	if (!_loadShaders())
		return false;

	const int voxelIndex = _scene->lightSpace.getVoxelLinearIndexFromPointInSpace(_scene->lightPos);

	_generateSidesFromVoxelIndex(voxelIndex, _sides);

	_edgeVisualizer.loadEdges(_edges);

	_updateSides();

	return true;
}

void HierarchicalSilhouetteRenderer::_initGL(unsigned int screenWidth, unsigned int screenHeight)
{
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(1, 1, 1, 1);
}

void HierarchicalSilhouetteRenderer::onUpdate(float timeSinceLastUpdateMs)
{
	
}

void HierarchicalSilhouetteRenderer::onKeyPressed(SDL_Keycode code)
{

}

void HierarchicalSilhouetteRenderer::onWindowRedraw(glm::mat4 cameraViewProjectionMatrix, glm::vec3 cameraPosition)
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//_drawSides(cameraViewProjectionMatrix);
	//_drawEdges(cameraViewProjectionMatrix);
	_drawScenePhong(cameraViewProjectionMatrix, cameraPosition);
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

	std::cout << "Scene has " << _pretransformedTriangles.size() * 3 << " triangles\n";
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

void HierarchicalSilhouetteRenderer::_generatePerEdgeVoxelInfo(const VoxelizedSpace& lightSpace)
{
	_edgeBitmasks.clear();
	
	for (const auto& edge : _edges)
	{
		assert(edge.second.size() <= 2);
		assert(edge.second.size() != 0);

		MultiBitArray ma(3, lightSpace.getNumVoxels());

		if (edge.second.size() == 1)
		{
			ma.setAllCells(EDGE_POTENTIALLY_SILHOUETTE); //To force multiplicity calculation due to sides winding
		}
		else
		{
			Plane p1, p2;
			
			_buildTrianglePlane(edge.first, edge.second[0], p1);
			_buildTrianglePlane(edge.first, edge.second[1], p2);

			const unsigned int numVoxels = lightSpace.getNumVoxels();
			for (unsigned int i = 0; i < numVoxels; ++i)
			{
				AABB voxel;
				lightSpace.getVoxelFromLinearIndex(i, voxel);
				int result1 = p1.testAABB(voxel);
				int result2 = p2.testAABB(voxel);
				
				int result = EDGE_NOT_SILHOUETTE;

				if (result1 == TEST_RESULT_INTERSECTS || result2 == TEST_RESULT_INTERSECTS)
					result = EDGE_POTENTIALLY_SILHOUETTE;
				else if ((result1*result2) < 0)
				{
					int multiplicity = _calcEdgeMultiplicity(edge, voxel.getMinPoint());
					result = EDGE_IS_SILHOUETTE_PLUS + (multiplicity<0);
				}

				ma.setCellContent(i, result);
			}
		}

		_edgeBitmasks.push_back(ma);
	}
}

void HierarchicalSilhouetteRenderer::_buildTrianglePlane(const Edge& edge, const glm::vec4& oppositeVertex, Plane& plane)
{
	const bool isCCW = EdgeExtractor::decodeEdgeWindingIsCCW(oppositeVertex);

	if (isCCW)
		plane.createFromPointsCCW(edge.lowerPoint, edge.higherPoint, glm::vec3(oppositeVertex));
	else
		plane.createFromPointsCCW(edge.higherPoint, edge.lowerPoint, glm::vec3(oppositeVertex));
}

void HierarchicalSilhouetteRenderer::_generateSidesFromVoxelIndex(unsigned int voxelLinearIndex, std::vector<glm::vec4>& sides)
{	
	int numSilhouetteEdges = 0;

	const auto numEdges = _edges.size();
	unsigned int i = 0;
	for (const auto& edge : _edges)
	{
		int result = _edgeBitmasks[i].getCellContent(voxelLinearIndex);

		if (EDGE_IS_SILHOUETTE(result))
		{
			const int multiplicitySign = (result == EDGE_IS_SILHOUETTE_PLUS) + (-1)*(result == EDGE_IS_SILHOUETTE_MINUS);
			_generatePushSideFromEdge(_scene->lightPos, edge.first, multiplicitySign, sides);
		}

		if (result == EDGE_POTENTIALLY_SILHOUETTE)
		{
			int r = _calcEdgeMultiplicity(edge, _scene->lightPos);
			if(r!=0)
				_generatePushSideFromEdge(_scene->lightPos, edge.first, r, sides);
		}

		++i;
	}
}

int HierarchicalSilhouetteRenderer::_calcEdgeMultiplicity(const std::pair<Edge, std::vector<glm::vec4> >& edgeInfo, const glm::vec3& lightPos) const
{
	const auto& edge = edgeInfo.first;
	const auto& oppositeVertices = edgeInfo.second;

	Plane lightPlane;
	lightPlane.createFromPointsCCW(edge.lowerPoint, edge.higherPoint, lightPos);
	int multiplicity = 0;
	for (const auto& oppositeVertex : oppositeVertices)
	{
		const float r = lightPlane.testPoint(glm::vec3(oppositeVertex));
		multiplicity += (r > 0) - (r < 0);
	}

	return multiplicity;
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

void HierarchicalSilhouetteRenderer::_initVoxelization()
{
	AABB space;
	space.setMinMaxPoints(glm::vec3(-10, -10, -10), glm::vec3(10, 10, 10));
	_scene->lightSpace.init(space, 5, 5, 5);
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