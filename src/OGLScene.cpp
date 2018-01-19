#include "OGLScene.hpp"

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.inl>

OGLScene::OGLScene()
{
	_IBO = 0;

	_VAO_withMaterials = 0;
	_VAO_withoutMaterials = 0;

	_VBO_vertices = 0;
	_VBO_normals = 0;
	_VBO_texcoords = 0;

	_SSBO_matrices = 0;
	_SSBO_materials = 0;
	_SSBO_textures = 0;

	_SSBO_matrices_size = 0;
	_SSBO_materials_size = 0;
	_SSBO_textures_size = 0;


	_sampler = 0;

	_numMeshes = 0;
}

OGLScene::~OGLScene()
{
	clear();
}


void OGLScene::loadScene(const std::shared_ptr<Scene> scene)
{
	if (!scene)
		return;

	_loadMaterials(scene->materials);

	_loadTextures(scene->textures);

	_loadMeshesMatrices(scene->meshes);

	_numMeshes = unsigned int(scene->meshes.size());
}

void OGLScene::clear()
{
	_clearMaterials();
	_clearTextures();
	_clearGeometry();

	_safeDeleteBuffer(_IBO);
}

void OGLScene::_loadTextures(const std::vector<Texture>& textures)
{
	assert(glGetError() == GL_NO_ERROR);
	std::vector<uint64_t> texHandles;

	_textures.resize(textures.size());
	texHandles.resize(textures.size());

	if (textures.size())
	{
		glGenTextures(GLsizei(_textures.size()), _textures.data());

		_createSampler();

		unsigned int i = 0;
		for (const auto& tex : textures)
		{
			glBindTexture(GL_TEXTURE_2D, _textures[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.data.get());
			glGenerateMipmap(GL_TEXTURE_2D);
			texHandles[i] = glGetTextureSamplerHandleARB(_textures[i], _sampler);
			glMakeTextureHandleResidentARB(texHandles[i]);

			++i;
		}
	}

	_createSSBO_Textures(texHandles);
	assert(glGetError() == GL_NO_ERROR);
}

void OGLScene::_createSampler()
{
	glGenSamplers(1, &_sampler);

	glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void OGLScene::_createSSBO_Textures(const std::vector<uint64_t>& texHandles)
{
	_SSBO_textures_size = sizeof(uint64_t) * texHandles.size();
	_SSBO_textures = _createGlBuffer(_SSBO_textures_size, texHandles.size() ? texHandles.data() : nullptr);
}

void OGLScene::_clearTextures()
{
	if (_textures.size())
	{
		glDeleteTextures(GLsizei(_textures.size()), _textures.data());
	}

	_safeDeleteBuffer(_SSBO_textures);
	_SSBO_textures_size = 0;

	glDeleteSamplers(1, &_sampler);
}

void OGLScene::_safeDeleteBuffer(GLuint& buffer)
{
	if (buffer)
	{
		glDeleteBuffers(1, &buffer);
		buffer = 0;
	}
}

void OGLScene::_loadMaterials(const std::vector<Material>& materials)
{
	_SSBO_materials_size = materials.size() * sizeof(Material);
	_SSBO_materials = _createGlBuffer(_SSBO_materials_size, materials.data());
}

void OGLScene::_loadMeshesMatrices(const std::vector<Mesh>& meshes)
{
	assert(glGetError() == GL_NO_ERROR);
	const unsigned int numVertices = _getSceneNumVertices(meshes);

	std::vector<glm::vec4> vertices;
	std::vector<glm::vec4> normals;
	std::vector<glm::vec2> tcoords;
	std::vector<float> matrices;

	vertices.reserve(numVertices);
	normals.reserve(numVertices);
	tcoords.reserve(numVertices);

	const unsigned int numMatricesPerMesh = 2;
	const unsigned int numFloatsPerMatrix = 16;
	const unsigned int numFloatsPerMesh = numMatricesPerMesh * numFloatsPerMatrix;
	const unsigned int matricesNumFloats = numFloatsPerMesh * meshes.size();
	
	matrices.resize(matricesNumFloats);

	unsigned int meshID = 0;
	for (const auto& mesh : meshes)
	{
		_appendTexcoords(mesh.texcoords, tcoords);

		memcpy(&matrices[numFloatsPerMesh * meshID + 0], glm::value_ptr(mesh.modelMatrix), numFloatsPerMatrix * sizeof(float));
		memcpy(&matrices[numFloatsPerMesh * meshID + numFloatsPerMatrix], glm::value_ptr(inverseTranspose(mesh.modelMatrix)), numFloatsPerMatrix * sizeof(float));

		const auto numMeshVertices = mesh.vertices.size();
		for (unsigned int i = 0; i < numMeshVertices; ++i)
		{
			glm::vec4 v = mesh.vertices[i];
			_embedUintIntoVectorW(v, meshID);

			glm::vec4 n = glm::vec4(mesh.normals[i], 0);
			_embedUintIntoVectorW(n, mesh.materialIndex);

			vertices.push_back(v);
			normals.push_back(n);
		}

		++meshID;
	}

	_SSBO_matrices_size = matricesNumFloats * sizeof(float);
	const auto ptr = matrices.data();
	_SSBO_matrices = _createGlBuffer(_SSBO_matrices_size, matrices.data());

	_VBO_vertices = _createGlBuffer(vertices.size() * 4 * sizeof(float), vertices.data());
	_VBO_normals = _createGlBuffer(normals.size() * 4 * sizeof(float), normals.data());
	_VBO_texcoords = _createGlBuffer(tcoords.size() * 2 * sizeof(float), tcoords.data());

	_createVAOWithNormalsTcoords();
	_createVAOWithoutNormalsTcoords();

	_createIBO(vertices.size());

	assert(glGetError() == GL_NO_ERROR);
}

void OGLScene::_createIBO(unsigned int numIndices)
{
	DrawArraysIndirectCommand cmd;

	cmd.baseInstance = 0;
	cmd.first = 0;
	cmd.instanceCount = 1;
	cmd.count = numIndices;

	_IBO = _createGlBuffer(sizeof(DrawArraysIndirectCommand), &cmd);
}

void OGLScene::_createVAOWithNormalsTcoords()
{
	assert(glGetError() == GL_NO_ERROR);
	glGenVertexArrays(1, &_VAO_withMaterials);

	glEnableVertexArrayAttribEXT(_VAO_withMaterials, SCENE_VERTEX_ATTRIB);
	glEnableVertexArrayAttribEXT(_VAO_withMaterials, SCENE_NORMAL_ATTRIB);
	glEnableVertexArrayAttribEXT(_VAO_withMaterials, SCENE_TCOORD_ATTRIB);

	glVertexArrayVertexAttribOffsetEXT(_VAO_withMaterials, _VBO_vertices, SCENE_VERTEX_ATTRIB, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexArrayVertexAttribOffsetEXT(_VAO_withMaterials, _VBO_normals, SCENE_NORMAL_ATTRIB, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexArrayVertexAttribOffsetEXT(_VAO_withMaterials, _VBO_texcoords, SCENE_TCOORD_ATTRIB, 2, GL_FLOAT, GL_FALSE, 0, 0);
	assert(glGetError() == GL_NO_ERROR);
}

void OGLScene::_createVAOWithoutNormalsTcoords()
{
	glGenVertexArrays(1, &_VAO_withoutMaterials);

	glEnableVertexArrayAttribEXT(_VAO_withoutMaterials, SCENE_VERTEX_ATTRIB);

	glVertexArrayVertexAttribOffsetEXT(_VAO_withoutMaterials, _VBO_vertices, SCENE_VERTEX_ATTRIB, 4, GL_FLOAT, GL_FALSE, 0, 0);
}

unsigned int OGLScene::_getSceneNumVertices(const std::vector<Mesh>& sceneMeshes) const
{
	unsigned int numVerts = 0;

	for (const auto& mesh : sceneMeshes)
		numVerts += unsigned int(mesh.vertices.size());

	return numVerts;
}

void OGLScene::_appendTexcoords(const std::vector<glm::vec2>& meshTcoords, std::vector<glm::vec2>& sceneTcoords)
{
	sceneTcoords.insert(sceneTcoords.end(), meshTcoords.begin(), meshTcoords.end());
}

void OGLScene::_embedUintIntoVectorW(glm::vec4& vector, unsigned int id) const
{
	*((unsigned int*)&(vector.w)) = id;
}

GLuint OGLScene::_createGlBuffer(GLsizei size, const void* data)
{
	assert(glGetError() == GL_NO_ERROR);
	
	GLuint buffer;
	glGenBuffers(1, &buffer);
	assert(glGetError() == GL_NO_ERROR);
	glNamedBufferDataEXT(buffer, size, data, GL_STATIC_READ);
	const auto e = glGetError();
	return buffer;
}

void OGLScene::_clearMaterials()
{
	_safeDeleteBuffer(_SSBO_materials);
	_SSBO_materials_size = 0;
}

void OGLScene::_clearGeometry()
{
	if (_VAO_withMaterials)
		glDeleteVertexArrays(1, &_VAO_withMaterials);

	if(_VAO_withoutMaterials)
		glDeleteVertexArrays(1, &_VAO_withoutMaterials);

	_safeDeleteBuffer(_VBO_vertices);
	_safeDeleteBuffer(_VBO_normals);
	_safeDeleteBuffer(_VBO_texcoords);
	_safeDeleteBuffer(_SSBO_matrices);
	
	_SSBO_matrices_size = 0;
}

void OGLScene::drawWithNormalsTcoords() const
{
	glBindVertexArray(_VAO_withMaterials);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _IBO);
	
	glDrawArraysIndirect(GL_TRIANGLES, nullptr);

	glBindVertexArray(0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

void OGLScene::drawWithoutNormalsTcoords() const
{
	glBindVertexArray(_VAO_withoutMaterials);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _IBO);

	glDrawArraysIndirect(GL_TRIANGLES, nullptr);

	glBindVertexArray(0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

GLuint OGLScene::getMatricesSSBO() const
{
	return _SSBO_matrices;
}

GLuint OGLScene::getMaterialsSSBO() const
{
	return _SSBO_materials;
}

GLuint OGLScene::getTexturesSSBO() const
{
	return _SSBO_textures;
}

GLsizei OGLScene::getMatricesSSBOSize() const
{
	return _SSBO_matrices_size;
}

GLsizei OGLScene::getMaterialsSSBOSize() const
{
	return _SSBO_materials_size;
}

GLsizei OGLScene::getTexturesSSBOSize() const
{
	return _SSBO_textures_size;
}