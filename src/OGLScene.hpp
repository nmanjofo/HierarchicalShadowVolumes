#pragma once

#include <GL/glew.h>

#include <vector>
#include <memory>
#include "Scene.hpp"

#define SCENE_VERTEX_ATTRIB 0
#define SCENE_NORMAL_ATTRIB 1
#define SCENE_TCOORD_ATTRIB 2



typedef  struct {
	GLuint  count;
	GLuint  instanceCount;
	GLuint  first;
	GLuint  baseInstance;
} DrawArraysIndirectCommand;

//Uses DrawArraysIndirect to draw scene
class OGLScene
{
public:
	OGLScene();
	~OGLScene();
	void loadScene(const std::shared_ptr<Scene> scene);
	void clear();

	void drawWithNormalsTcoords() const;
	void drawWithoutNormalsTcoords() const;

	GLuint getMatricesSSBO() const;
	GLuint getMaterialsSSBO() const;
	GLuint getTexturesSSBO() const;

	GLsizei getMatricesSSBOSize() const;
	GLsizei getMaterialsSSBOSize() const;
	GLsizei getTexturesSSBOSize() const;

private:
	void _loadMaterials(const std::vector<Material>& materials);

	void _loadMeshesMatrices(const std::vector<Mesh>& meshes);
		void _embedUintIntoVectorW(glm::vec4& vector, unsigned int id) const;
		unsigned int _getSceneNumVertices(const std::vector<Mesh>& sceneMeshes) const;
		void _appendTexcoords(const std::vector<glm::vec2>& meshTcoords, std::vector<glm::vec2>& sceneTcoords);
		void _createVAOWithNormalsTcoords();
		void _createVAOWithoutNormalsTcoords();
		void _createIBO(unsigned int numIndices);

	void _loadTextures(const std::vector<Texture>& textures);
		void _createSampler();
		void _createSSBO_Textures(const std::vector<uint64_t>& texHandles);
	
	void _clearTextures();
	void _clearMaterials();
	void _clearGeometry();

	GLuint _createGlBuffer(GLsizei size, const void* data);
	void _safeDeleteBuffer(GLuint& buffer);

	GLuint _IBO;

	GLuint _VAO_withMaterials;
	GLuint _VAO_withoutMaterials;

	GLuint _VBO_vertices;
	GLuint _VBO_normals;
	GLuint _VBO_texcoords;

	GLuint _SSBO_matrices;
	GLsizei _SSBO_matrices_size;

	GLuint _SSBO_materials;
	GLsizei _SSBO_materials_size;

	GLuint _SSBO_textures;
	GLsizei _SSBO_textures_size;

	GLuint _sampler;

	std::vector<GLuint> _textures;

	unsigned int _numMeshes;
};