//=================================================================
// Container for two vertices and identifiers of the neighbouring triangles
//=================================================================

#include "Edge.h"
#include "Triangle.h"
#include "Vertex.h"

Edge::Edge()
{
}

Edge::Edge(int _index, Vertex* _vertex1, Vertex* _vertex2, Triangle* _triangle)
{
	m_index = _index;
	m_vertex1 = _vertex1;
	m_vertex2 = _vertex2;
	m_triangle = _triangle;
}

Edge::~Edge()
{
}