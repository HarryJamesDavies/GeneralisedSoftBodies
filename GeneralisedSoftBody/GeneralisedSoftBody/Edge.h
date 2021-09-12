//=================================================================
// Container for two vertices and identifiers of the neighbouring triangles
//=================================================================

#ifndef _EDGE_H_
#define _EDGE_H_

class Triangle;
struct Vertex;

class Edge
{
public:
	Edge();
	Edge(int _index, Vertex* _vertex1, Vertex* _vertex2, Triangle* _triangle);
	~Edge();

	int m_index;

	Vertex* m_vertex1;
	Vertex* m_vertex2;

	//Indices of vertices within the triangle
	int m_localVertex1Index;
	int m_localVertex2Index;

	//Index of triangle adjacent to the edge and shared edge within the triangle
	int m_neighbouringTriangleIndex = -1;
	int m_sharedEdgeIndex = -1;

	//Triangle the edge belongs to
	Triangle* m_triangle;

private:

};

#endif;
