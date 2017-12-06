#pragma once

#include "CommonTypes.hpp"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <map>
#include <vector>

class EdgeVisualizer
{
public:
	EdgeVisualizer();
	~EdgeVisualizer();
	
	void loadEdges(const std::map<Edge, std::vector<glm::vec4> >& edges);
	
	void drawEdges() const;

	void clear();

private:
	GLuint	_VAO;
	GLuint  _VBO;

	unsigned int _numEdges;
};