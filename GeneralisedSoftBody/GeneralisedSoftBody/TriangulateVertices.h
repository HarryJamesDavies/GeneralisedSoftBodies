//=================================================================
// Generate polygons based on vertices and indices data
//=================================================================

#ifndef _TRIANGULATE_VERTICES_H_
#define _TRIANGULATE_VERTICES_H_
#include <list>
#include <d3d11_1.h>
#include "SimpleMath.h"

class Triangle;
class Edge;
struct Vertex;

using namespace std;

class TriangulateVertices
{
public:
	TriangulateVertices();
	~TriangulateVertices();

	void CalculateTriangles(std::list<Triangle*>* _triangles, Vertex* _vertices, WORD* _indices, UINT _numPrims);

	void CompareEdgeOverlap(Edge* _edge, Triangle* _triangle);
	void CompareEdges(Edge* _edge1, Edge* _edge2);
	bool CompareVertices(Vertex* _vertex1, Vertex* _vertex2);

	void RemoveDuplicates(std::list<Triangle*>* _triangles);

private:
};
#endif;
