#pragma once

#include "Edge.hpp"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <vector>

class EdgeVisualizer
{
public:
	EdgeVisualizer();
	~EdgeVisualizer();
	
	void loadEdges(const EDGE_CONTAINER_TYPE& edges);
	
	void drawEdges() const;

	void clear();

private:
	GLuint	_VAO;
	GLuint  _VBO;

	unsigned int _numEdges;
};