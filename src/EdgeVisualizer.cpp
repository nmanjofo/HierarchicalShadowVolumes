#include "EdgeVisualizer.hpp"

EdgeVisualizer::EdgeVisualizer()
{
	_VAO = _VBO = _numEdges = 0;
}

EdgeVisualizer::~EdgeVisualizer()
{
	clear();
}

void EdgeVisualizer::loadEdges(const std::map<Edge, std::vector<glm::vec4> >& edges)
{
	if (_numEdges)
		clear();

	assert(glGetError() == GL_NO_ERROR);
	_numEdges = unsigned int(edges.size());

	std::vector<glm::vec4> e;
	e.resize(_numEdges * 2);

	unsigned int i = 0;
	for (const auto& edge : edges)
	{
		e[i + 0] = glm::vec4(edge.first.lowerPoint, 1);
		e[i + 1] = glm::vec4(edge.first.higherPoint, 1);

		i += 2;
	}

	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);
	
	auto r = glGetError();
	glEnableVertexArrayAttribEXT(_VAO, 0);
	glNamedBufferDataEXT(_VBO, 4 * sizeof(float)*e.size(), e.data(), GL_STATIC_DRAW);
	r = glGetError();
	glVertexArrayVertexAttribOffsetEXT(_VAO, _VBO, 0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	r = glGetError();
	assert(glGetError() == GL_NO_ERROR);
}

void EdgeVisualizer::clear()
{
	if (_numEdges)
	{
		glDeleteVertexArrays(1, &_VAO);
		glDeleteBuffers(1, &_VBO);
		_numEdges = 0;
	}
}

void EdgeVisualizer::drawEdges() const
{
	if (_numEdges)
	{
		assert(glGetError() == GL_NO_ERROR);
		glBindVertexArray(_VAO);
		glDrawArrays(GL_LINES, 0, 2 * _numEdges);
		glBindVertexArray(0);
		assert(glGetError() == GL_NO_ERROR);
	}
}