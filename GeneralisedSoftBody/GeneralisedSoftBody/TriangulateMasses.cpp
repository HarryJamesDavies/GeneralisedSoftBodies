//=================================================================
// Generate mass/spring structure based on polygons
//=================================================================

#include "TriangulateMasses.h"
#include "Triangle.h"
#include "Edge.h"
#include "Vertex.h"
#include "MSTriangle.h"
#include "MSMass.h"
#include "MSMesh.h"
#include "MSEdge.h"


TriangulateMasses::TriangulateMasses()
{
}

TriangulateMasses::~TriangulateMasses()
{
}

void TriangulateMasses::CalculateTriangles(MSMesh* _mesh, int _maxVertices, int _maxTriangles, std::list<Triangle*>::iterator _trianglesIter, std::list<Triangle*>::iterator _trianglesEnd)
{
	m_checkList = new bool[_maxVertices];
	bool fixed = false;
	int index = 0;

	//Generates temporary masses based on the position of each vertex in _triangles.
	for (auto triangleBegin = _trianglesIter; triangleBegin != _trianglesEnd; triangleBegin++)
	{
		index = CheckEdge((*triangleBegin)->m_edge1, index);
		index = CheckEdge((*triangleBegin)->m_edge2, index);
		index = CheckEdge((*triangleBegin)->m_edge3, index);
	}

	//Get vertices connected to assigned vertices by edges that don’t share
	//the mass’ position and get associated triangles of these masses.
	for (auto iter = m_list.begin(); iter != m_list.end(); iter++)
	{
		//Loop through the vertices of every temporary mass.
		for (auto vertexIter = (*iter)->m_vertices.begin(); vertexIter != (*iter)->m_vertices.end(); vertexIter++)
		{
			int vertexIndex = -1;

			//Loop through all triangles checking for shared edges.
			for (auto triangleBegin = _trianglesIter; triangleBegin != _trianglesEnd; triangleBegin++)
			{
				//Edge 1.
				vertexIndex = CheckSharedEdge((*vertexIter), (*triangleBegin)->m_edge1);
				if (vertexIndex != -1)
				{
					//Add the vertex that was not matched on the shared edge.
					(*iter)->m_neighbourVertices.push_back(vertexIndex);

					//Check whether the temporary mass already holds the index of the neighbouring triangle.
					if (!CheckExistingTriangle((*triangleBegin)->m_index, (*iter)->m_triangles.begin(), (*iter)->m_triangles.end()))
					{
						//If not add the index of the triangle to neighbouring triangle list.
						(*iter)->m_triangles.push_back((*triangleBegin)->m_index);
					}
					vertexIndex = -1;
				}

				//Edge 2.
				vertexIndex = CheckSharedEdge((*vertexIter), (*triangleBegin)->m_edge2);
				if (vertexIndex != -1)
				{
					//Add the vertex that was not matched on the shared edge.
					(*iter)->m_neighbourVertices.push_back(vertexIndex);

					//Check whether the temporary mass already holds the index of the neighbouring triangle.
					if (!CheckExistingTriangle((*triangleBegin)->m_index, (*iter)->m_triangles.begin(), (*iter)->m_triangles.end()))
					{
						//If not add the index of the triangle.
						(*iter)->m_triangles.push_back((*triangleBegin)->m_index);
					}
					vertexIndex = -1;
				}

				//Edge 3.
				vertexIndex = CheckSharedEdge((*vertexIter), (*triangleBegin)->m_edge3);
				if (vertexIndex != -1)
				{
					//Add the vertex that was not matched on the shared edge.
					(*iter)->m_neighbourVertices.push_back(vertexIndex);

					//Check whether the temporary mass already holds the index of the neighbouring triangle.
					if (!CheckExistingTriangle((*triangleBegin)->m_index, (*iter)->m_triangles.begin(), (*iter)->m_triangles.end()))
					{
						//If not add the index of the triangle.
						(*iter)->m_triangles.push_back((*triangleBegin)->m_index);
					}
					vertexIndex = -1;
				}
			}
		}
	}

	//Removes duplicated assigned vertices.
	RemoveDuplicates();

	//Get all neighbouring temporary masses.
	for (auto iter = m_list.begin(); iter != m_list.end(); iter++)
	{
		//Loop through all neighbouring vertices.
		for (auto neighbour = (*iter)->m_neighbourVertices.begin(); neighbour != (*iter)->m_neighbourVertices.end(); neighbour++)
		{
			CheckNeighbourList((*neighbour), (*iter)->m_index, iter);
		}
	}

	//Generate mass triangles by looping through all polygons.
	for (auto triangleBegin = _trianglesIter; triangleBegin != _trianglesEnd; triangleBegin++)
	{
		CreateTriangle(_mesh, (*triangleBegin));
	}

	//Find natural starting point.
	bool startPoint = false;
	list<MassList*>::iterator startMass;

	//Loop through all temporary masses and check for one with exactly one asscoiated triangle.
	for (auto iter = m_list.begin(); iter != m_list.end(); iter++)
	{
		//If located, set as start point due naturally only having strucutral neighbours.
		if (!startPoint && (*iter)->m_triangles.size() == 1)
		{
			(*iter)->m_set = true;
			startPoint = true;
			startMass = iter;
		}
		else if ((*iter)->m_triangles.size() == 1)
		{
			(*iter)->m_set = true;
		}
	}

	//Create starting point if no natural points occur.
	if (!startPoint)
	{
		//Loop through all temporary masses and check for one with exactly six asscoiated triangles.
		for (auto iter = m_list.begin(); iter != m_list.end(); iter++)
		{
			if (!startPoint && (*iter)->m_triangles.size() == 6)
			{
				startPoint = true;
				startMass = iter;

				//Remove the fourth neighbour mass.
				auto neighbourIter = (*iter)->m_neighbours.begin();
				for (int i = 1; i <= 4; i++)
				{
					neighbourIter++;
				}

				(*iter)->m_neighbours.remove((*neighbourIter));

				//Remove the second neighbour mass.
				neighbourIter = (*iter)->m_neighbours.begin();
				for (int i = 1; i <= 2; i++)
				{
					neighbourIter++;
				}

				(*iter)->m_neighbours.remove((*neighbourIter));

				(*iter)->m_set = true;
			}
		}
	}

	//Remove all non-structural neighbour masses.
	if (startPoint)
	{
		CheckNeighboursSet(_mesh, startMass);
	}

	//Get the lowest point on the mesh.
	float fixedPoint = FindFixPoint(_mesh->m_fixDimension, _mesh->m_fixIndex);

	//Loop through temporary masses generating masses.
	for (auto iter = m_list.begin(); iter != m_list.end(); iter++)
	{
		if (_mesh->m_fixFormat == FixFormat::All)
		{
			fixed = true;
		}
		else if (_mesh->m_fixFormat == FixFormat::None)
		{
			fixed = false;
		}
		else if (_mesh->m_fixFormat == FixFormat::Single)
		{
			if ((*iter)->m_index == _mesh->m_fixIndex)
			{
				fixed = true;
			}
		}
		else
		{
			//Lock the physics of any mass at the same Y position as first mass.
			if (CheckFixed(_mesh->m_fixDimension, _mesh->m_fixFormat, (*iter)->m_pos, fixedPoint))
			{
				fixed = true;
			}
		}

		//Creates new mass/spring mass.
		_mesh->m_masses.push_back(new MSMass((*iter)->m_index, 1.0f, (*iter)->m_pos, _mesh, fixed, true));

		//Assign all vertices associated with temporary mass to generated mass.
		for (auto vertexIter = (*iter)->m_vertices.begin(); vertexIter != (*iter)->m_vertices.end(); vertexIter++)
		{
			m_verticesToAssign.push_back((*vertexIter));
		}
		AssignVertices(_mesh->m_masses.back());

		fixed = false;
	}

	//Assign temporary masses neighbours to equivalent mass/spring mass.
	auto massIter = _mesh->m_masses.begin();
	for (auto iter = m_list.begin(); iter != m_list.end(); iter++)
	{
		for (auto neighbours = (*iter)->m_neighbours.begin(); neighbours != (*iter)->m_neighbours.end(); neighbours++)
		{
			(*massIter)->AddNeighbour(GetMass(_mesh, (*neighbours)));
		}
		massIter++;
	}

	m_list.clear();
}

//Checks whether vertex1 of _edge is in a unique position, generates temp mass is new or adds to existing if shared. Returns current mass index.
int TriangulateMasses::CheckEdge(Edge* _edge, int _index)
{
	int index = _index;

	//Checks whether any existing mass shares this position.
	auto iter = CheckExistingList(_edge->m_vertex1->Pos);

	if (iter == m_list.end())
	{
		//If new position generates new temp mass at this position.
		m_list.push_back(new MassList());
		m_list.back()->m_index = index;
		m_list.back()->m_pos = _edge->m_vertex1->Pos;
		m_list.back()->m_triangles = list<int>();

		//Adds vertex1 of _edge to new temp mass' vertex list.
		m_list.back()->m_vertices.push_back(_edge->m_triangle->m_vertexIndex[_edge->m_localVertex1Index]);

		//Ticks flag to indicate that vertex1 of _edge has been assigned.
		m_checkList[_edge->m_triangle->m_vertexIndex[_edge->m_localVertex1Index]] = true;

		//Loops through all list of vertices that share a position with vertex1 of _edge.
		for (auto vertexIter = _edge->m_triangle->m_vertexData.m_vertex1.begin(); vertexIter != _edge->m_triangle->m_vertexData.m_vertex1.end(); vertexIter++)
		{
			//Prevents pre-assigned vertices from being assigned twice.
			if (!m_checkList[(*vertexIter)])
			{
				//Adds vertex to temp mass' vertex list.
				m_list.back()->m_vertices.push_back((*vertexIter));

				//Ticks flag to indicate that vertex has been assigned.
				m_checkList[(*vertexIter)] = true;
			}
		}

		//Increase index indicating number of temp masses.
		index++;
	}
	else
	{
		//Adds vertex1 of _edge to existing temp mass' vertex list.
		(*iter)->m_vertices.push_back(_edge->m_triangle->m_vertexIndex[_edge->m_localVertex1Index]);

		//Ticks flag to indicate that vertex1 of _edge has been assigned.
		m_checkList[_edge->m_triangle->m_vertexIndex[_edge->m_localVertex1Index]] = true;

		//Loops through all list of vertices that share a position with vertex1 of _edge.
		for (auto vertexIter = _edge->m_triangle->m_vertexData.m_vertex1.begin(); vertexIter != _edge->m_triangle->m_vertexData.m_vertex1.end(); vertexIter++)
		{
			//Prevents pre-assigned vertices from being assigned twice.
			if (!m_checkList[(*vertexIter)])
			{
				//Adds vertex to temp mass' vertex list.
				(*iter)->m_vertices.push_back((*vertexIter));

				//Ticks flag to indicate that vertex has been assigned.
				m_checkList[(*vertexIter)] = true;
			}
		}
	}

	return index;
}

//Checks whether any existing mass shares this position.
std::list<MassList*>::iterator TriangulateMasses::CheckExistingList(Vector3 _pos)
{
	//Check whether the distance between the vertex and any existing mass are less than 0.0001.
	for (auto it = m_list.begin(); it != m_list.end(); it++)
	{
		//If true, then they share a position.
		if (Vector3::DistanceSquared((*it)->m_pos, _pos) <= 0.0001)
		{
			return it;
		}
	}
	return m_list.end();
}

//Determine if _edge is contrusted from the vertex with _vertexIndex
int TriangulateMasses::CheckSharedEdge(int _vertexIndex, Edge* _edge)
{
	//Check both vertices of the edge, if either match, return the other vertex index.
	if (_edge->m_triangle->m_vertexIndex[_edge->m_localVertex1Index] == _vertexIndex)
	{
		return _edge->m_triangle->m_vertexIndex[_edge->m_localVertex2Index];
	}
	else if (_edge->m_triangle->m_vertexIndex[_edge->m_localVertex2Index] == _vertexIndex)
	{
		return _edge->m_triangle->m_vertexIndex[_edge->m_localVertex1Index];
	}

	return -1;
}

//Determines whether triangle has already been created.
bool TriangulateMasses::CheckExistingTriangle(int _triangleIndex, std::list<int>::iterator _begin, std::list<int>::iterator _end)
{
	for (_begin; _begin != _end; _begin++)
	{
		if ((*_begin) == _triangleIndex)
		{
			return true;
		}
	}
	return false;
}

//Check and remove duplicated neighbour vertices.
void TriangulateMasses::RemoveDuplicates()
{
	for (auto iter = m_list.begin(); iter != m_list.end(); iter++)
	{
		list<int> verticesToRemove;
		verticesToRemove.clear();

		//Loop through each temporary mass's vertices checking for duplications.
		for (auto vertexIterA = (*iter)->m_vertices.begin(); vertexIterA != (*iter)->m_vertices.end(); vertexIterA++)
		{
			//Start checking loop at the following temporary mass.
			auto vertexIterB = vertexIterA;
			vertexIterB++;

			//Ensures that there is always a mass to check off against.
			if (vertexIterA != (*iter)->m_vertices.end())
			{
				for (vertexIterB; vertexIterB != (*iter)->m_vertices.end(); vertexIterB++)
				{
					//If two vertex indices are equal, there are duplications.
					if ((*vertexIterA) == (*vertexIterB))
					{
						//Add vertex index to be remove list.
						verticesToRemove.push_back((*vertexIterB));
					}
				}
			}
		}

		//Loop through remove list and remove and duplicated vertex indices.
		for (auto removeIter = verticesToRemove.begin(); removeIter != verticesToRemove.end(); removeIter++)
		{
			auto it = std::find((*iter)->m_vertices.begin(), (*iter)->m_vertices.end(), (*removeIter));

			if ((*iter)->m_vertices.size() != 1)
			{
				if (it != (*iter)->m_vertices.end())
				{
					(*iter)->m_vertices.erase(it);
				}
			}
		}
	}
}

//Find temporary mass that _vertexIndex belongs to and add that temporary mass to neighbouring mass list.
void TriangulateMasses::CheckNeighbourList(int _vertexIndex, int _listIndex, std::list<MassList*>::iterator _start)
{
	//Start from the temporary following the current one.
	_start++;
	for (auto iter = _start; iter != m_list.end(); iter++)
	{
		//Loop through all vertices and check whether _vertexIndex belongs to it.
		for (auto vertices = (*iter)->m_vertices.begin(); vertices != (*iter)->m_vertices.end(); vertices++)
		{
			if ((*vertices) == _vertexIndex)
			{
				//Check whether this temporary mass has already been assigned.
				if (!CheckExistingNeighbour(_listIndex, iter))
				{
					//If not, assign neighbour.
					(*iter)->m_neighbours.push_back(_listIndex);
					return;
				}
			}
		}
	}
	return;
}

//Check whether this temporary mass has already been assigned.
bool TriangulateMasses::CheckExistingNeighbour(int _index, std::list<MassList*>::iterator _list)
{
	//Loop through all neighbouring temporary masses to check for _index.
	for (auto iter = (*_list)->m_neighbours.begin(); iter != (*_list)->m_neighbours.end(); iter++)
	{
		if ((*iter) == _index)
		{
			return true;
		}
	}
	return false;
}

//Generate a mass triangle from polygon.
void TriangulateMasses::CreateTriangle(MSMesh* _mesh, Triangle* _triangle)
{
	MassList* massA = nullptr;
	MassList* massB = nullptr;
	MassList* massC = nullptr;

	//Loop through all temporary masses and find mass containing _triangle.
	for (auto listIter = m_list.begin(); listIter != m_list.end(); listIter++)
	{
		for (auto triangleIter = (*listIter)->m_triangles.begin(); triangleIter != (*listIter)->m_triangles.end(); triangleIter++)
		{
			if ((*triangleIter) == _triangle->m_index)
			{
				//Loop through vertices to find neighbouring temporary masses containing one vertex from triangle.
				for (auto vertexIter = (*listIter)->m_vertices.begin(); vertexIter != (*listIter)->m_vertices.end(); vertexIter++)
				{
					if ((massA == nullptr) && (_triangle->m_vertexIndex[0]) == (*vertexIter))
					{
						massA = (*listIter);
					}
					else if ((massB == nullptr) && (_triangle->m_vertexIndex[1]) == (*vertexIter))
					{
						massB = (*listIter);
					}
					else if ((massC == nullptr) && (_triangle->m_vertexIndex[2]) == (*vertexIter))
					{
						massC = (*listIter);
					}
				}
			}
		}
	}

	//If all temporary masses have been assigned, generate mass triangle.
	if (massA && massB && massC)
	{
		MSTriangle* triangle = new MSTriangle();
		triangle->m_index = _triangle->m_index;
		triangle->m_massA = massA->m_index;
		triangle->m_massB = massB->m_index;
		triangle->m_massC = massC->m_index;

		MSEdge* edgeA = new MSEdge();
		edgeA->m_index = 0;
		edgeA->m_massAIndex = massA->m_index;
		edgeA->m_massBIndex = massB->m_index;
		edgeA->m_triangle = triangle;
		triangle->m_edgeA = edgeA;

		MSEdge* edgeB = new MSEdge();
		edgeB->m_index = 1;
		edgeB->m_massAIndex = massB->m_index;
		edgeB->m_massBIndex = massC->m_index;
		edgeB->m_triangle = triangle;
		triangle->m_edgeB = edgeB;

		MSEdge* edgeC = new MSEdge();
		edgeC->m_index = 2;
		edgeC->m_massAIndex = massC->m_index;
		edgeC->m_massBIndex = massA->m_index;
		edgeC->m_triangle = triangle;
		triangle->m_edgeC = edgeC;

		_mesh->m_triangles.push_back(triangle);
	}
}

//Recursive function that checks whether the neighbours of this temporary mass only have structural neighbours remaining.
void TriangulateMasses::CheckNeighboursSet(MSMesh* _mesh, std::list<MassList*>::iterator _list)
{
	//Loops through neighbouring masses and checks whether they're non-structural neighbours have been removed.
	for (auto indexIter = (*_list)->m_neighbours.begin(); indexIter != (*_list)->m_neighbours.end(); indexIter++)
	{
		auto neighbourIter = GetNeighbour(*indexIter);
		if (!(*neighbourIter)->m_set)
		{
			SetNeighboursByReference(_mesh, neighbourIter, _list);
		}
	}
}

//Returns the temporary mass associated with _index.
std::list<MassList*>::iterator TriangulateMasses::GetNeighbour(int _index)
{
	for (auto iter = m_list.begin(); iter != m_list.end(); iter++)
	{
		if ((*iter)->m_index == _index)
		{
			return iter;
		}
	}
	return m_list.end();
}

//Removes non-structural neighbours relative to a temporary mass thats has already been processed.
void TriangulateMasses::SetNeighboursByReference(MSMesh* _mesh, std::list<MassList*>::iterator _list, std::list<MassList*>::iterator _reference)
{
	//Removes any neighbour shared by both _reference and _list.
	int neighbourToBeRemoved = -1;
	for (auto neighbourIterA = (*_list)->m_neighbours.begin(); neighbourIterA != (*_list)->m_neighbours.end(); neighbourIterA++)
	{
		for (auto neighbourIterB = (*_reference)->m_neighbours.begin(); neighbourIterB != (*_reference)->m_neighbours.end(); neighbourIterB++)
		{
			if ((*neighbourIterA) == (*neighbourIterB))
			{
				neighbourToBeRemoved = (*neighbourIterA);
			}
		}
	}

	if (neighbourToBeRemoved != -1)
	{
		(*_list)->m_neighbours.remove(neighbourToBeRemoved);
	}

	MSTriangle* inBetweenTriangle = nullptr;


	//Loop through triangles and find the one that contains _reference but not removed neighbour.
	for (auto triangleIter = (*_list)->m_triangles.begin(); triangleIter != (*_list)->m_triangles.end(); triangleIter++)
	{
		MSTriangle* triangle = GetTriangle(_mesh, (*triangleIter));

		if (triangle != nullptr)
		{
			if (triangle->ContainMass((*_reference)->m_index) && !triangle->ContainMass(neighbourToBeRemoved))
			{
				//Assign as triangle between two outlying triangles.
				inBetweenTriangle = triangle;
			}
		}
	}

	if (inBetweenTriangle != nullptr)
	{
		//Return the mass that is not _list and not _reference in the inbetween triangle.
		int inBetweenMass = inBetweenTriangle->GetOtherMass((*_list)->m_index, (*_reference)->m_index);
		MSTriangle* removeTriangle = nullptr;

		//Find the triangle contain inBetweenMass but not the _reference. 
		for (auto triangleIter = (*_list)->m_triangles.begin(); triangleIter != (*_list)->m_triangles.end(); triangleIter++)
		{
			MSTriangle* triangle = GetTriangle(_mesh, (*triangleIter));

			if (triangle != nullptr)
			{
				if (triangle->ContainMass(inBetweenMass) && !triangle->ContainMass((*_reference)->m_index))
				{
					removeTriangle = triangle;
				}
			}
		}

		if (removeTriangle != nullptr)
		{
			//Get and remove the mass that is not _list and not inBetweenMass in removeTriangle.
			neighbourToBeRemoved = removeTriangle->GetOtherMass((*_list)->m_index, inBetweenMass);

			if (neighbourToBeRemoved != -1)
			{
				(*_list)->m_neighbours.remove(neighbourToBeRemoved);
			}
		}
	}

	(*_list)->m_set = true;

	//Recursively check neighbours the same way.
	CheckNeighboursSet(_mesh, _list);
}

//Returns mass triangle associated with _triangleIndex.
MSTriangle* TriangulateMasses::GetTriangle(MSMesh* _mesh, int _triangleIndex)
{
	for (auto triangleIter = _mesh->m_triangles.begin(); triangleIter != _mesh->m_triangles.end(); triangleIter++)
	{
		if ((*triangleIter)->m_index == _triangleIndex)
		{
			return (*triangleIter);
		}
	}
	return nullptr;
}

//Assign all verteices in m_verticesToAssign to _mass.
void TriangulateMasses::AssignVertices(MSMass* _mass)
{
	for (auto it = m_verticesToAssign.begin(); it != m_verticesToAssign.end(); it++)
	{
		_mass->BindVertex((*it));
	}

	m_verticesToAssign.clear();
}

//Returns the fixed point on the mesh.
float TriangulateMasses::FindFixPoint(FixDimension _dimension, int _index)
{
	float fixPoint = 0.0f;
	int index = 0;

	//Gets the value of the fix point on the mesh relative to fix dimension
	for (auto listIter = m_list.begin(); listIter != m_list.end(); listIter++)
	{
		if (index == _index)
		{
			switch (_dimension)
			{
			case FixDimension::X:
			{
				fixPoint = (*listIter)->m_pos.x;
				break;
			}
			case FixDimension::Y:
			{
				fixPoint = (*listIter)->m_pos.y;
				break;
			}
			case FixDimension::Z:
			{
				fixPoint = (*listIter)->m_pos.z;
				break;
			}
			default:
			{
				return false;
				break;
			}
			}
		}
		index++;
	}

	return fixPoint;
}

//Checks if mass is a fixed point.
bool TriangulateMasses::CheckFixed(FixDimension _dimension, FixFormat _format, Vector3 _pos, float _fixedPoint)
{
	float checkPoint = 0.0f;

	//Gets the point to check against relative to fix dimension
	switch (_dimension)
	{
	case FixDimension::X:
	{
		checkPoint = _pos.x;
		break;
	}
	case FixDimension::Y:
	{
		checkPoint = _pos.y;
		break;
	}
	case FixDimension::Z:
	{
		checkPoint = _pos.z;
		break;
	}
	default:
	{
		return false;
		break;
	}
	}

	//Checks values based on fixing format
	switch (_format)
	{
	case FixFormat::Level:
	{
		if ((_fixedPoint == checkPoint))
		{
			return true;
		}
		break;
	}
	case FixFormat::Above:
	{
		if ((_fixedPoint <= checkPoint))
		{
			return true;
		}
		break;
	}
	case FixFormat::Below:
	{
		if ((_fixedPoint >= checkPoint))
		{
			return true;
		}
		break;
	}
	default:
	{
		return false;
	}
	}

	return false;
}

//Return mass assigned with _index.
MSMass* TriangulateMasses::GetMass(MSMesh* _mesh, int _index)
{
	for (auto it = _mesh->m_masses.begin(); it != _mesh->m_masses.end(); it++)
	{
		if ((*it)->GetIndex() == _index)
		{
			return (*it);
		}
	}

	return nullptr;
}