//=================================================================
// Generate mass/spring structure based on polygons
//=================================================================

#ifndef _TRIANGULATE_MASSES_H_
#define _TRIANGULATE_MASSES_H_
#include <list>
#include "d3d11_1.h"
#include "SimpleMath.h"
#include "FixDimension.h"
#include "FixFormat.h"

using namespace std;

class MSMesh;
class MSMass;
class MSTriangle;
class Triangle;
class Edge;

using namespace DirectX;
using namespace DirectX::SimpleMath;

//Temporary structure to hold in progress data
struct MassList
{
	int m_index;
	bool m_set;
	Vector3 m_pos;
	list<int> m_vertices;
	list<int> m_neighbourVertices;
	list<int> m_triangles;
	list<int> m_neighbours;
};

class TriangulateMasses
{
public:
	TriangulateMasses();
	~TriangulateMasses();

	void CalculateTriangles(MSMesh* _mesh, int _maxVertices, int _maxTriangles, std::list<Triangle*>::iterator _trianglesIter, std::list<Triangle*>::iterator _trianglesEnd);

private:
	void CreateTriangle(MSMesh* _mesh, Triangle* _triangle);
	MSTriangle* GetTriangle(MSMesh* _mesh, int _triangleIndex);

	int CheckEdge(Edge* _edge, int _index);
	int CheckSharedEdge(int _vertexIndex, Edge* _edge);

	std::list<MassList*>::iterator CheckExistingList(Vector3 _pos);
	bool CheckExistingTriangle(int _triangleIndex, std::list<int>::iterator _begin, std::list<int>::iterator _end);
	bool CheckExistingNeighbour(int _index, std::list<MassList*>::iterator _list);

	void CheckNeighbourList(int _vertexIndex, int _listIndex, std::list<MassList*>::iterator _start);
	void CheckNeighboursSet(MSMesh* _mesh, std::list<MassList*>::iterator _list);

	std::list<MassList*>::iterator GetNeighbour(int _index);
	void SetNeighboursByReference(MSMesh* _mesh, std::list<MassList*>::iterator _list, std::list<MassList*>::iterator _reference);

	void RemoveDuplicates();

	MSMass* GetMass(MSMesh* _mesh, int _index);

	void AssignVertices(MSMass* _mass);

	float FindFixPoint(FixDimension _dimension, int _index);
	bool CheckFixed(FixDimension _dimension, FixFormat _format, Vector3 _pos, float _fixedPoint);

	list<MassList*> m_list;

	//Checklist for assigned vertices
	bool *m_checkList;

	//List of vertices to assign to a mass
	list<int> m_verticesToAssign;
};

#endif;