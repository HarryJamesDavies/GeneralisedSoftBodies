//=================================================================
// Triangle structure used inconjunction with vertices and indices to define meshes
//=================================================================
#include "Triangle.h"
#include "Edge.h"

Triangle::Triangle()
{
}

Triangle::Triangle(int _index, int _vertex1Index, int _vertex2Index, int _vertex3Index, Edge* _edge1, Edge* _edge2, Edge* _edge3)
{
	m_index = _index;

	m_vertexIndex[0] = _vertex1Index;
	m_vertexIndex[1] = _vertex2Index;
	m_vertexIndex[2] = _vertex3Index;

	m_edge1 = _edge1;
	m_edge2 = _edge2;
	m_edge3 = _edge3;

	m_edge1->m_triangle = this;
	m_edge2->m_triangle = this;
	m_edge3->m_triangle = this;
}

Triangle::~Triangle()
{
	delete(m_edge1);
	delete(m_edge2);
	delete(m_edge3);

	m_edge1 = nullptr;
	m_edge2 = nullptr;
	m_edge3 = nullptr;
}