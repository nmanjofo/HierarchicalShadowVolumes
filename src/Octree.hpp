#pragma once

#include <memory>

#define OCTREE_NUM_CHILDREN 8

class Node
{
public:
	Node();



private:
	std::shared_ptr<Node> _children[OCTREE_NUM_CHILDREN];
};



class Octree
{
	
};