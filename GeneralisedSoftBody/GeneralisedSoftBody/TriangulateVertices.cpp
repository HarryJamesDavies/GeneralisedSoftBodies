//=================================================================
// Generate polygons based on vertices and indices data
//=================================================================

#include "TriangulateVertices.h"
#include "Triangle.h"
#include "Edge.h"
#include "Vertex.h"


TriangulateVertices::TriangulateVertices()
{
}

TriangulateVertices::~TriangulateVertices()
{
}

//Generate polygons based on meshes vertices and indices.
void TriangulateVertices::CalculateTriangles(std::list<Triangle*>* _triangles, Vertex* _vertices, WORD* _indices, UINT _numPrims)
{
	//Create triangles utilising list of indices.
	for (UINT i = 0; i < _numPrims; i++)
	{
		//Get next set of three indices that make up the triangle.
		WORD V1 = _indices[3 * i];
		WORD V2 = _indices[3 * i + 1];
		WORD V3 = _indices[3 * i + 2];

		//Create triangle edges using indices to find the triangles vertices.
		Edge* edge1 = new Edge();
		Edge* edge2 = new Edge();
		Edge* edge3 = new Edge();

		edge1->m_vertex1 = &_vertices[V1];
		edge1->m_vertex2 = &_vertices[V2];
		edge1->m_index = 0;
		edge1->m_localVertex1Index = 0;
		edge1->m_localVertex2Index = 1;

		edge2->m_vertex1 = &_vertices[V2];
		edge2->m_vertex2 = &_vertices[V3];
		edge2->m_index = 1;
		edge2->m_localVertex1Index = 1;
		edge2->m_localVertex2Index = 2;

		edge3->m_vertex1 = &_vertices[V3];
		edge3->m_vertex2 = &_vertices[V1];
		edge3->m_index = 2;
		edge3->m_localVertex1Index = 2;
		edge3->m_localVertex2Index = 0;

		//Create triangle from three edges.
		_triangles->push_back(new Triangle(i, V1, V2, V3, edge1, edge2, edge3));
	}

	//Loop through triangles to find shared edges between triangles, neighbouring triangles 
	//and generate three lists of verter indices that share a position with one of the triangle's vertices.
	int index = 1;
	for (auto it = _triangles->begin(); it != _triangles->end(); it++)
	{
		//Start comparison after previously check triangle to reduce repitition.
		auto iter = _triangles->begin();
		for (int i = 1; i <= index; i++)
		{
			iter++;
		}

		//Compare all edges in the triangle.
		for (iter; iter != _triangles->end(); iter++)
		{
			CompareEdgeOverlap((*it)->m_edge1, (*iter));

			CompareEdgeOverlap((*it)->m_edge2, (*iter));

			CompareEdgeOverlap((*it)->m_edge3, (*iter));
		}

		index++;
	}

	//Remove any duplications of shared vertex indices.
	RemoveDuplicates(_triangles);
}

//Compare whether _edge overlaps with any edges in _triangle.
void TriangulateVertices::CompareEdgeOverlap(Edge* _edge, Triangle* _triangle)
{
	CompareEdges(_edge, _triangle->m_edge1);

	CompareEdges(_edge, _triangle->m_edge2);

	CompareEdges(_edge, _triangle->m_edge3);
}

//Compare whether _edge1 and edge2 share two vertex postions
void TriangulateVertices::CompareEdges(Edge* _edge1, Edge* _edge2)
{
	//If vertex1 of edge1 and vertex1 of edge share a position, as well as, vertex2 of edge1 
	//and vertex2 of edge2 share a positon or vice-verses. Then the two triangles share an edge.
	if (((_edge1->m_vertex1->Pos == _edge2->m_vertex1->Pos) && (_edge1->m_vertex2->Pos == _edge2->m_vertex2->Pos)
		|| (_edge1->m_vertex1->Pos == _edge2->m_vertex2->Pos) && (_edge1->m_vertex2->Pos == _edge2->m_vertex1->Pos)))
	{
		//Add edge2 index as edge1 shared edge index and its triangle as neighbouring triangle.
		_edge1->m_sharedEdgeIndex = _edge2->m_index;
		_edge1->m_neighbouringTriangleIndex = _edge2->m_triangle->m_index;

		//Add edge1 index as edge2 shared edge index and its triangle as neighbouring triangle.
		_edge2->m_sharedEdgeIndex = _edge1->m_index;
		_edge2->m_neighbouringTriangleIndex = _edge1->m_triangle->m_index;

		//Determine which vertex pair the edges share. Add opposing vertex indices to relevant shared vertex list.
		if (CompareVertices(_edge1->m_vertex1, _edge2->m_vertex1))
		{
			_edge1->m_triangle->m_vertexData.GetList(_edge1->m_localVertex1Index)->push_back(_edge2->m_triangle->m_vertexIndex[_edge2->m_localVertex1Index]);
			_edge1->m_triangle->m_vertexData.GetList(_edge1->m_localVertex2Index)->push_back(_edge2->m_triangle->m_vertexIndex[_edge2->m_localVertex2Index]);

			_edge2->m_triangle->m_vertexData.GetList(_edge2->m_localVertex1Index)->push_back(_edge1->m_triangle->m_vertexIndex[_edge1->m_localVertex1Index]);
			_edge2->m_triangle->m_vertexData.GetList(_edge2->m_localVertex2Index)->push_back(_edge1->m_triangle->m_vertexIndex[_edge1->m_localVertex2Index]);
		}
		else
		{
			_edge1->m_triangle->m_vertexData.GetList(_edge1->m_localVertex1Index)->push_back(_edge2->m_triangle->m_vertexIndex[_edge2->m_localVertex2Index]);
			_edge1->m_triangle->m_vertexData.GetList(_edge1->m_localVertex2Index)->push_back(_edge2->m_triangle->m_vertexIndex[_edge2->m_localVertex1Index]);

			_edge2->m_triangle->m_vertexData.GetList(_edge2->m_localVertex1Index)->push_back(_edge1->m_triangle->m_vertexIndex[_edge1->m_localVertex2Index]);
			_edge2->m_triangle->m_vertexData.GetList(_edge2->m_localVertex2Index)->push_back(_edge1->m_triangle->m_vertexIndex[_edge1->m_localVertex1Index]);
		}
	}
}

//Determine whether two vertices overlap.
bool TriangulateVertices::CompareVertices(Vertex* _vertex1, Vertex* _vertex2)
{
	if (_vertex1->Pos == _vertex2->Pos)
	{
		return true;
	}
	return false;
}

//Remove any duplication of shared vertex indices.
void TriangulateVertices::RemoveDuplicates(std::list<Triangle*>* _triangles)
{
	for (auto iter = _triangles->begin(); iter != _triangles->end(); iter++)
	{
		//Loops through all shared vertex lists.
		list<int> verticesToRemove;
		verticesToRemove.clear();
		for (int index = 0; index <= 2; index++)
		{
			//Loops through all vertex indices in each list.
			for (auto vertexIterA = (*iter)->m_vertexData.GetList(index)->begin(); vertexIterA != (*iter)->m_vertexData.GetList(index)->end(); vertexIterA++)
			{
				//Start checking loop at the vertex index that follows the current one.
				auto vertexIterB = vertexIterA;
				vertexIterB++;
				if (vertexIterA != (*iter)->m_vertexData.GetList(index)->end())
				{
					//Ensures that they is always an entery to check aganist.
					for (vertexIterB; vertexIterB != (*iter)->m_vertexData.GetList(index)->end(); vertexIterB++)
					{
						//If the incides are equal there is a duplication and the index is recorded.
						if ((*vertexIterA) == (*vertexIterB))
						{
							verticesToRemove.push_back((*vertexIterB));
						}
					}
				}
			}

			//Loops through all flag vertex indices and removes them from the shared vertices list.
			for (auto removeIter = verticesToRemove.begin(); removeIter != verticesToRemove.end(); removeIter++)
			{
				auto it = std::find((*iter)->m_vertexData.GetList(index)->begin(), (*iter)->m_vertexData.GetList(index)->end(), (*removeIter));

				if (it != (*iter)->m_vertexData.GetList(index)->end())
				{
					(*iter)->m_vertexData.GetList(index)->erase(it);
				}
			}

			verticesToRemove.clear();
		}	
	}
}