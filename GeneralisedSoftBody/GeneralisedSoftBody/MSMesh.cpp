//=================================================================
// Combined mesh of masses and springs
//=================================================================

#include "MSMesh.h"
#include "MSMass.h"
#include "MSSpring.h"
#include "VertexMSO.h"
#include "GlobalData.h"
#include "Triangle.h"
#include "Edge.h"
#include "MSTriangle.h"
#include "MSEdge.h"
#include "TriangulateMasses.h"
#include <fstream>
#include <sstream>
#include <string>

#include <iostream>
#include <fstream>

MSMesh::MSMesh(VertexMSO* _parent, int _fixAll, FixFormat _format, FixDimension _dimension)
{
	m_parent = _parent;
	m_verticesToAssign.clear();

	m_fixIndex = _fixAll;
	m_fixFormat = _format;
	m_fixDimension = _dimension;
}

MSMesh::~MSMesh()
{
	m_masses.clear();
	m_verticesToAssign.clear();
	m_parent = nullptr;
}

//Algorithm used to generate 1D chain
void MSMesh::CreateMasses1D(int _maxVertices, int _width, int _height, int _numSectionsX)
{
	//Determines the ratio of vertices per section
	int numberOfVetricesPerSection = _maxVertices / _numSectionsX;

	//Inital mass position at the top of the mesh
	int index = 0;
	int currentSection = 0;
	Vector3 massPos = Vector3(static_cast<float>((_width / 2)), static_cast<float>(-(currentSection * _height)), 0.0f);
	bool fixed = true;

	//Assign a section of mesh per mass
	for (int i = 0; i <= _numSectionsX; i++)
	{
		//Moves mass postion to the start of next section
		massPos.y = static_cast<float>(-currentSection * _height);

		//Mass generated at the top of the next section
		m_masses.push_back(new MSMass(index, 1.0f, massPos, this, fixed, true));

		//As long algorithm is not at the bottom of the mesh, add next three vertices
		if (i != _numSectionsX)
		{
			m_verticesToAssign.push_back(0 + (currentSection * numberOfVetricesPerSection));
			m_verticesToAssign.push_back(2 + (currentSection * numberOfVetricesPerSection));
			m_verticesToAssign.push_back(3 + (currentSection * numberOfVetricesPerSection));
		}

		//As long algorithm is not at the top of the mesh, add previous three vertices
		if (i != 0)
		{
			m_verticesToAssign.push_back(-1 + (currentSection * numberOfVetricesPerSection));
			m_verticesToAssign.push_back(-5 + (currentSection * numberOfVetricesPerSection));
			m_verticesToAssign.push_back(-2 + (currentSection * numberOfVetricesPerSection));
		}

		//Assign vertices to current mass
		AssignVertices(m_masses.back());

		index++;
		currentSection++;
		fixed = false;
	}

	return;
}

//Generate masses of the surface layer
void MSMesh::CreateSurfaceMasses(int _maxVertices, int _maxTriangles, std::list<Triangle*>::iterator _trianglesIter, std::list<Triangle*>::iterator _trianglesEnd)
{
	TriangulateMasses* math = new TriangulateMasses();

	//Pass over to algorithm in Math systems
	math->CalculateTriangles(this, _maxVertices, _maxTriangles, _trianglesIter, _trianglesEnd);
	delete(math);

	return;
}

//Generate masses of the interal layer
void MSMesh::CreateInternalMasses()
{
	Vector3 massPos = Vector3::Zero;

	//Duplicate the surface layers masses along the masses normal
	for (auto massIter = m_masses.begin(); massIter != m_masses.end(); massIter++)
	{
		massPos = (*massIter)->GetPos() + (*massIter)->GetNormal();
		m_internalMasses.push_back(new MSMass((*massIter)->GetIndex(), 1.0f, massPos, this, false, true));

		//Setup surface layer mass as opposite mass, can be used to calculate normals
		(*massIter)->AssignOpposingMass(m_internalMasses.back());
		m_internalMasses.back()->AssignOpposingMass((*massIter));
	}

	//Assign structural neighbours relative to the surface masses neighbours
	for (auto massIter = m_internalMasses.begin(); massIter != m_internalMasses.end(); massIter++)
	{
		for (auto neighbourIter = (*massIter)->GetOpposingMass()->GetNeighboursBeginning(); 
			neighbourIter != (*massIter)->GetOpposingMass()->GetNeighbourEnd(); neighbourIter++)
		{
			(*massIter)->AddNeighbour((*neighbourIter)->GetOpposingMass());
		}
	}

	return;
}

//Assign list of vertices to a specific mass
void MSMesh::AssignVertices(MSMass* _mass)
{
	//Binds all vertices on m_verticesToAssign list to _mass
	for (auto it = m_verticesToAssign.begin(); it != m_verticesToAssign.end(); it++)
	{
		_mass->BindVertex((*it));
	}

	m_verticesToAssign.clear();
}

//Create springs connecting 1D chains
void MSMesh::CreateSprings1D(int _numSectionsX)
{
	int massAIter = 1;
	int massBIter = 0;

	//Get first pair of masses
	MSMass* massA = GetMass(massAIter);
	MSMass* massB = GetMass(massBIter);
	m_springs.push_back(new MSSpring(massA, massB, m_springs.size()));

	//Loop through other masses connect them via springs
	for (int iter = 1; iter <= _numSectionsX - 1; iter++)
	{
		massAIter++;
		
		massA = GetMass(massAIter);
		massB = GetMass(massBIter);
		m_springs.push_back(new MSSpring(massA, massB, m_springs.size()));

		massBIter++;

		massA = GetMass(massAIter);
		massB = GetMass(massBIter);
		m_springs.push_back(new MSSpring(massA, massB, m_springs.size()));
	}

	return;
}

//Connect surface masses to the structural neighbours and neighbour's structural neighbours via springs
void MSMesh::CreateSurfaceSprings()
{
	for (auto massIter = m_masses.begin(); massIter != m_masses.end(); massIter++)
	{
		//Loop through mass's structural neighbours
		for (auto neighbourIter = (*massIter)->GetNeighboursBeginning(); neighbourIter != (*massIter)->GetNeighbourEnd(); neighbourIter++)
		{
			//Connect surface masses to the structural neighbours
			if (!CheckExistingSpring((*massIter), (*neighbourIter)))
			{
				m_springs.push_back(new MSSpring((*massIter), (*neighbourIter), m_springs.size()));
			}

			//Loop through neighbours's structural neighbours
			for (auto neighboursNeighbourIter = (*neighbourIter)->GetNeighboursBeginning(); neighboursNeighbourIter != (*neighbourIter)->GetNeighbourEnd(); neighboursNeighbourIter++)
			{
				//Connect mass to neighbour's structural neighbours via springs
				if (!CheckExistingSpring((*massIter), (*neighboursNeighbourIter)))
				{
					m_springs.push_back(new MSSpring((*massIter), (*neighboursNeighbourIter), m_springs.size()));
				}
			}
		}
	}

	int iter = 0;
	iter++;

	return;
}

//Connect internal masses to the structural neighbours and neighbour's structural neighbours via springs
void MSMesh::CreateInternalSprings()
{
	MSMass* massA = nullptr;
	MSMass* massB = nullptr;

	for (auto massIter = m_internalMasses.begin(); massIter != m_internalMasses.end(); massIter++)
	{
		//Loop through mass's structural neighbours
		for (auto neighbourIter = (*massIter)->GetNeighboursBeginning(); neighbourIter != (*massIter)->GetNeighbourEnd(); neighbourIter++)
		{
			//Connect surface masses to the structural neighbours
			if (!CheckExistingSpring((*massIter), (*neighbourIter)))
			{
				m_springs.push_back(new MSSpring((*massIter), (*neighbourIter), m_springs.size()));
			}

			//Loop through neighbours's structural neighbours
			for (auto neighboursNeighbourIter = (*neighbourIter)->GetNeighboursBeginning(); neighboursNeighbourIter != (*neighbourIter)->GetNeighbourEnd(); neighboursNeighbourIter++)
			{
				//Connect mass to neighbour's structural neighbours via springs
				if (!CheckExistingSpring((*massIter), (*neighboursNeighbourIter)))
				{
					m_springs.push_back(new MSSpring((*massIter), (*neighboursNeighbourIter), m_springs.size()));
				}
			}
		}

		//Connect internal to surface mass
		m_springs.push_back(new MSSpring((*massIter), (*massIter)->GetOpposingMass(), m_springs.size()));

		//Loop through surface mass's structural neighbours
		for (auto neighbourIter = (*massIter)->GetOpposingMass()->GetNeighboursBeginning(); neighbourIter != (*massIter)->GetOpposingMass()->GetNeighbourEnd(); neighbourIter++)
		{
			//Connect intermass to surface mass's structural neighbours
			if (!CheckExistingSpring((*massIter), (*neighbourIter)))
			{
				m_springs.push_back(new MSSpring((*massIter), (*neighbourIter), m_springs.size()));
			}

			//Loop through surface mass's neighbours's structural neighbours
			for (auto neighboursNeighbourIter = (*neighbourIter)->GetNeighboursBeginning(); neighboursNeighbourIter != (*neighbourIter)->GetNeighbourEnd(); neighboursNeighbourIter++)
			{
				//Connect mass to surface mass's neighbour's structural neighbours via springs
				if (!CheckExistingSpring((*massIter), (*neighboursNeighbourIter)))
				{
					m_springs.push_back(new MSSpring((*massIter), (*neighboursNeighbourIter), m_springs.size()));
				}
			}
		}
	}
}

//Determines whether two masses are already connected via springs
bool MSMesh::CheckExistingSpring(MSMass* _massA, MSMass* _massB)
{
	for (auto springIter = m_springs.begin(); springIter != m_springs.end(); springIter++)
	{
		if ((*springIter)->ContainsMass(_massA) && (*springIter)->ContainsMass(_massB))
		{
			return true;
		}
	}
	return false;
}

//Updates the normal of each mass
void MSMesh::UpdateNormals()
{
	//Resets surface masses normals to Vector3::Zero
	for (auto massIter = m_masses.begin(); massIter != m_masses.end(); massIter++)
	{
		(*massIter)->SetNormal(Vector3::Zero);
	}

	//Calculates the normals of every surface triangle
	for (auto triangleIter = m_triangles.begin(); triangleIter != m_triangles.end(); triangleIter++)
	{
		CalculateNormals((*triangleIter));
	}

	//Sets mass's collective normal relative to connected surface triangle's normal
	for (auto massIter = m_masses.begin(); massIter != m_masses.end(); massIter++)
	{
		(*massIter)->SetNormal((*massIter)->GetNormal());
	}

	//Resets Internals masses normals to Vector3::Zero
	for (auto massIter = m_internalMasses.begin(); massIter != m_internalMasses.end(); massIter++)
	{
		(*massIter)->SetNormal(Vector3::Zero);
	}

	//Sets internal masses normals relative to opposing surface mass
	for (auto massIter = m_internalMasses.begin(); massIter != m_internalMasses.end(); massIter++)
	{
		Vector3 normal = (*massIter)->GetOpposingMass()->GetPos() - (*massIter)->GetPos();
		normal.Normalize();
		(*massIter)->SetNormal(normal);
	}
}

//Calculates the spring force of each spring
void MSMesh::UpdateSprings(GlobalData* _GD)
{
	//Resets spring force to zero
	for (auto it = m_masses.begin(); it != m_masses.end(); it++)
	{
		(*it)->SetSpringForce(Vector3(0.0f, 0.0f, 0.0f));
	}

	//Calculates springs force
	for (auto it = m_springs.begin(); it != m_springs.end(); it++)
	{
		(*it)->CalculateForce(_GD);
	}
}

//Calculates the next frame's mass and vertex displacement 
void MSMesh::UpdateMasses(GlobalData* _GD)
{
	//Calculates the next frame's surface mass displacement 
	for (auto it = m_masses.begin(); it != m_masses.end(); it++)
	{
		(*it)->CalculateDisplacement(_GD);
	}

	//Calculates the next frame's internal mass displacement 
	for (auto it = m_internalMasses.begin(); it != m_internalMasses.end(); it++)
	{
		(*it)->CalculateDisplacement(_GD);
	}
}

//Applies masses and vertex displacement
void MSMesh::ApplyChanges()
{
	//Applies surface masses and vertex displacement
	for (auto it = m_masses.begin(); it != m_masses.end(); it++)
	{
		(*it)->ApplyDisplacement();
	}

	//Applies internal masses displacement
	for (auto it = m_internalMasses.begin(); it != m_internalMasses.end(); it++)
	{
		(*it)->ApplyDisplacement();
	}
}

//Return surface mass with index: _index
MSMass* MSMesh::GetMass(int _index)
{
	for (auto it = m_masses.begin(); it != m_masses.end(); it++)
	{
		if ((*it)->GetIndex() == _index)
		{
			return (*it);
		}
	}

	return nullptr;
}

//Return internal mass with index: _index
MSMass* MSMesh::GetExternalMass(int _index)
{
	for (auto it = m_internalMasses.begin(); it != m_internalMasses.end(); it++)
	{
		if ((*it)->GetIndex() == _index)
		{
			return (*it);
		}
	}

	return nullptr;
}

//Calculates the normals a specific triangle
void MSMesh::CalculateNormals(MSTriangle* _triangle)
{
	MSMass* massA = GetMass(_triangle->m_massA);
	MSMass* massB = GetMass(_triangle->m_massB);
	MSMass* massC = GetMass(_triangle->m_massC);

	//Build normals
	Vector3 norm;
	Vector3 vec1 = massB->GetPos() - massA->GetPos();
	Vector3 vec2 = massC->GetPos() - massA->GetPos();
	norm = vec1.Cross(vec2);
	norm.Normalize();

	//Calculates the weighting of each normal relative to internal angle of the vertex
	float angle = CalculateAngle(massA->GetPos(), massB->GetPos(), massC->GetPos());
	massA->AddNormal(norm * angle);

	angle = CalculateAngle(massB->GetPos(), massC->GetPos(), massA->GetPos());
	massB->AddNormal(norm * angle);

	angle = CalculateAngle(massC->GetPos(), massA->GetPos(), massB->GetPos());
	massC->AddNormal(norm * angle);
}

//Calculates the internal angle between two edges
float MSMesh::CalculateAngle(Vector3 _angleVertex, Vector3 _nextVertexA, Vector3 _prevVertex)
{
	//Gets edges of the triangle
	Vector3 edgeA = _angleVertex - _nextVertexA;
	Vector3 edgeB = _angleVertex - _prevVertex;

	edgeA.Normalize();
	edgeB.Normalize();

	//Gets the angle between these edges
	float angle = edgeA.Dot(edgeB);

	//Ensures value stays between 1 and -1 before appling inverse cosine
	if (angle > 1.0f)
	{
		angle = 1.0f;
	}
	else if (angle < -1.0f)
	{
		angle = -1.0f;
	}

	return acos(angle);
}

//Resets masses to intial position when the mesh was generated
void MSMesh::ResetMasses()
{
	//Moves the surface masses back their orignal positions and reset the mass's forces
	for (auto massIter = m_masses.begin(); massIter != m_masses.end(); massIter++)
	{
		(*massIter)->SetPos((*massIter)->GetOrignalPos());
		(*massIter)->SetSpringForce(Vector3::Zero);
		(*massIter)->SetVelocity(Vector3::Zero);
	}

	//Moves the internal masses back their orignal positions and reset the mass's forces
	for (auto massIter = m_internalMasses.begin(); massIter != m_internalMasses.end(); massIter++)
	{
		(*massIter)->SetPos((*massIter)->GetOrignalPos());
		(*massIter)->SetSpringForce(Vector3::Zero);
		(*massIter)->SetVelocity(Vector3::Zero);
	}
}

//Reads in mass/spring structure from MSO Text file 
void MSMesh::CreateFromFile(const char* _fileName)
{
	std::ifstream infile;
	infile.open(_fileName);

	//Ensures the file can be opened
	while (infile.good())
	{
		//Reads in every line from the Text file
		std::string line;
		while (std::getline(infile, line))
		{
			std::istringstream in(line);

			std::string type;
			in >> type;

			if (type == "SURMASSES:")
			{
				//Reads in surface masses
				int numberOfSurfaceMasses = 0;
				in >> numberOfSurfaceMasses;
				for (int surfaceIter = 0; surfaceIter <= numberOfSurfaceMasses - 1; surfaceIter++)
				{
					//Gets surface mass data
					Vector3 pos = Vector3(0.0, 0.0, 0.0);
					float mass = 0.0;
					int numberOfVertices = 0;
					int opposite = 0;
					int fixed = 0;

					std::getline(infile, line);

					//Gets position
					std::getline(infile, line);
					std::istringstream posLine(line);
					posLine.ignore(line.length(), ':');
					posLine >> pos.x >> pos.y >> pos.z;

					//Gets mass value
					std::getline(infile, line);
					std::istringstream massLine(line);
					massLine.ignore(line.length(), ':');
					massLine >> mass;

					//Gets fixed flag
					std::getline(infile, line);
					std::istringstream fixedLine(line);
					fixedLine.ignore(line.length(), ':');
					fixedLine >> fixed;

					//Generates mass based on parameters
					m_masses.push_back(new MSMass(surfaceIter, mass, pos, this, static_cast<bool>(fixed), true));

					//Gets vertices and binds them to current mass
					std::getline(infile, line);
					std::istringstream vertexLine(line);
					vertexLine.ignore(line.length(), ':');
					vertexLine >> numberOfVertices;

					std::getline(infile, line);
					std::istringstream single(line);
					for (int vertexIter = 0; vertexIter <= numberOfVertices - 1; vertexIter++)
					{
						int vertexIndex = 0;
						single >> vertexIndex;
						m_masses.back()->BindVertex(vertexIndex);
					}

					//Skips neighbours until all masses are generated
					std::getline(infile, line);
					std::getline(infile, line);
					std::getline(infile, line);
				}
			}
			else if (type == "INTMASSES:")
			{
				//Reads in surface masses
				int numberOfInternalMasses = 0;
				in >> numberOfInternalMasses;
				for (int internalIter = 0; internalIter <= numberOfInternalMasses - 1; internalIter++)
				{
					//Gets surface mass data
					Vector3 pos = Vector3(0.0, 0.0, 0.0);
					float mass = 0.0;
					int fixed = 0;

					std::getline(infile, line);

					//Gets position
					std::getline(infile, line);
					std::istringstream posLine(line);
					posLine.ignore(line.length(), ':');
					posLine >> pos.x >> pos.y >> pos.z;

					//Gets mass value
					std::getline(infile, line);
					std::istringstream massLine(line);
					massLine.ignore(line.length(), ':');
					massLine >> mass;

					//Gets fixed flag
					std::getline(infile, line);
					std::istringstream fixedLine(line);
					fixedLine.ignore(line.length(), ':');
					fixedLine >> fixed;
					
					//Generates mass based on parameters
					m_internalMasses.push_back(new MSMass(internalIter, mass, pos, this, static_cast<bool>(fixed), false));

					//Skips neighbours until all masses are generated
					std::getline(infile, line);
					std::getline(infile, line);
					std::getline(infile, line);

				}
			}
			else if (type == "TRIANGLES:")
			{
				//Reads in surface triangles
				int numberOfTriangle = 0;
				in >> numberOfTriangle;
				for (int triangleIter = 0; triangleIter <= numberOfTriangle - 1; triangleIter++)
				{
					int massAIndex = -1;
					int massBIndex = -1;
					int massCIndex = -1;

					//Gets mass indices
					std::getline(infile, line);
					std::istringstream single(line);
					single.ignore(line.length(), ':');

					single >> massAIndex >> massBIndex >> massCIndex;

					//Generate new triangle and edges based on mass indices
					MSTriangle* triangle = new MSTriangle();
					triangle->m_index = triangleIter;
					triangle->m_massA = massAIndex;
					triangle->m_massB = massBIndex;
					triangle->m_massC = massCIndex;

					MSEdge* edgeA = new MSEdge();
					edgeA->m_index = 0;
					edgeA->m_massAIndex = massAIndex;
					edgeA->m_massBIndex = massBIndex;
					edgeA->m_triangle = triangle;
					triangle->m_edgeA = edgeA;

					MSEdge* edgeB = new MSEdge();
					edgeB->m_index = 1;
					edgeB->m_massAIndex = massBIndex;
					edgeB->m_massBIndex = massCIndex;
					edgeB->m_triangle = triangle;
					triangle->m_edgeB = edgeB;

					MSEdge* edgeC = new MSEdge();
					edgeC->m_index = 2;
					edgeC->m_massAIndex = massCIndex;
					edgeC->m_massBIndex = massAIndex;
					edgeC->m_triangle = triangle;
					triangle->m_edgeC = edgeC;

					m_triangles.push_back(triangle);
				}
			}
			else if (type == "SPRINGS:")
			{
				//Reads in spring data
				int numberOfSprings = 0;
				in >> numberOfSprings;
				for (int springIter = 0; springIter <= numberOfSprings - 1; springIter++)
				{
					int massAIndex = -1;
					int massBIndex = -1;

					//Get mass indices
					std::getline(infile, line);
					std::istringstream single(line);
					single.ignore(line.length(), ':');

					single >> massAIndex >> massBIndex;

					//Generates new spring connected between these two masses
					m_springs.push_back(new MSSpring(GetMass(massAIndex), GetMass(massBIndex), springIter));
				}
			}
		}
	}

	infile.close();

	//Re-Opens file to read in mass neighbours now that they are generated
	infile.open(_fileName);

	//Ensures the file is safe to read from
	while (infile.good())
	{
		//Reads in every line from the Text file
		std::string line;
		while (std::getline(infile, line))
		{
			std::istringstream in(line);

			std::string type;
			in >> type;

			if (type == "SURMASSES:")
			{
				//Reads in surface mass neighbours
				int numberOfSurfaceMasses = 0;
				in >> numberOfSurfaceMasses;
				for (int surfaceIter = 0; surfaceIter <= numberOfSurfaceMasses - 1; surfaceIter++)
				{
					int numberOfNeighbours = 0;
					int opposite = 0;

					MSMass* mass = GetMass(surfaceIter);

					//Skips all mass data until neighbour data
					std::getline(infile, line);
					std::getline(infile, line);
					std::getline(infile, line);
					std::getline(infile, line);
					std::getline(infile, line);
					std::getline(infile, line);

					//Reads in structural neighbour indices
					std::getline(infile, line);
					std::istringstream single(line);
					single.ignore(line.length(), ':');
					single >> numberOfNeighbours;
					std::getline(infile, line);
					std::istringstream neighbourLine(line);
					for (int neighbourIter = 0; neighbourIter <= numberOfNeighbours - 1; neighbourIter++)
					{
						int neighbour = -1;
						neighbourLine >> neighbour;
						mass->AddNeighbour(GetMass(neighbour));
					}

					//Reads in opposing mass index
					std::getline(infile, line);
					std::istringstream oppositeLine(line);
					oppositeLine.ignore(line.length(), ':');
					oppositeLine >> opposite;
					mass->AssignOpposingMass(GetMass(opposite));
				}
			}
			else if (type == "INTMASSES:")
			{
				//Reads in internal mass neighbours
				int numberOfInternalMasses = 0;
				in >> numberOfInternalMasses;
				for (int internalIter = 0; internalIter <= numberOfInternalMasses - 1; internalIter++)
				{
					int numberOfNeighbours = 0;
					int opposite = 0;

					MSMass* mass = GetExternalMass(internalIter);

					//Skips all mass data until neighbour data
					std::getline(infile, line);
					std::getline(infile, line);
					std::getline(infile, line);
					std::getline(infile, line);

					//Reads in structural neighbour indices
					std::getline(infile, line);
					std::istringstream single(line);
					single.ignore(line.length(), ':');
					single >> numberOfNeighbours;
					std::getline(infile, line);
					std::istringstream neighbourLine(line);
					for (int neighbourIter = 0; neighbourIter <= numberOfNeighbours - 1; neighbourIter++)
					{
						int neighbour = -1;
						neighbourLine >> neighbour;
						mass->AddNeighbour(GetMass(neighbour));
					}

					//Reads in opposing mass index
					std::getline(infile, line);
					std::istringstream oppositeLine(line);
					oppositeLine.ignore(line.length(), ':');
					oppositeLine >> opposite;
					mass->AssignOpposingMass(GetMass(opposite));
				}
			}
		}
	}

	infile.close();
}

//Saves mesh data to MSO Text file
void MSMesh::SaveMesh()
{
	//Generates file name from mesh name
	std::string fileName = "../Assets/MSO/" + m_parent->GetName() + ".txt";
	std::ofstream outfile(fileName);

	//Saves out mesh name
	outfile << m_parent->GetName() << std::endl;
	outfile << std::endl;

	//Saves out whether object is 3D
	outfile << "3D: " << (int)m_3D << std::endl;
	outfile << std::endl;

	//Saves out vertex data
	outfile << "VERTICES: " << m_parent->GetVertexCount() << std::endl;
	for (int vertexIter = 0; vertexIter <= m_parent->GetVertexCount() - 1; vertexIter++)
	{
		outfile << "VERTEX " << vertexIter << ": " << std::endl;
		outfile << "	Pos: " << std::fixed << m_parent->GetInitialVertex(vertexIter)->Pos.x << " " << m_parent->GetInitialVertex(vertexIter)->Pos.y << " " << m_parent->GetInitialVertex(vertexIter)->Pos.z << std::endl;
		outfile << "	Colour: " << std::fixed << m_parent->GetInitialVertex(vertexIter)->Color.R() << " " << m_parent->GetInitialVertex(vertexIter)->Color.G() << " " << m_parent->GetInitialVertex(vertexIter)->Color.B() << " " << m_parent->GetInitialVertex(vertexIter)->Color.A() << std::endl;
	}
	outfile << std::endl;

	//Saves out index data
	outfile << "INDICES: " << m_parent->GetIndexCount() << std::endl;
	for (int indexIter = 0; indexIter <= m_parent->GetIndexCount() - 1; indexIter++)
	{
		outfile << indexIter << ": " << std::fixed << m_parent->GetIndex(indexIter) << std::endl;
	}
	outfile << std::endl;

	//Saves out surface mass data
	outfile << "SURMASSES: " << m_masses.size() << std::endl;
	for (auto massIter = m_masses.begin(); massIter != m_masses.end(); massIter++)
	{
		//Saves out position, mass value and whether mass is fixed
		outfile << "SURMASS " << (*massIter)->GetIndex() << ": " << std::endl;
		outfile << "	Pos: " << std::fixed << (*massIter)->GetOrignalPos().x << " " << (*massIter)->GetOrignalPos().y << " " << (*massIter)->GetOrignalPos().z << std::endl;
		outfile << "	Mass: " << std::fixed << (*massIter)->GetMass() << std::endl;
		outfile << "	Fixed: " << std::fixed << (int)(*massIter)->GetFixed() << std::endl;

		//Saves out bound vertex indices
		outfile << "	Vertices: " << std::fixed << (*massIter)->GetNumberOfVertices() << std::endl;
		outfile << "		";
		for (auto vertexIter = (*massIter)->GetVerticesBegin(); vertexIter != (*massIter)->GetVerticesEnd(); vertexIter++)
		{
			outfile << std::fixed << (*vertexIter) << " ";
		}
		outfile << std::endl;

		//Saves out structural neighbour indices
		outfile << "	Neighbours: " << std::fixed << (*massIter)->GetNumberOfNeighbours() << std::endl;
		outfile << "		";
		for (auto neighbourIter = (*massIter)->GetNeighboursBeginning(); neighbourIter != (*massIter)->GetNeighbourEnd(); neighbourIter++)
		{
			outfile << std::fixed << (*neighbourIter)->GetIndex() << " ";
		}
		outfile << std::endl;

		//Saves out opposing mass index
		MSMass* opposingMass = (*massIter)->GetOpposingMass();
		if (opposingMass)
		{
			outfile << "	Opposing: " << std::fixed << opposingMass->GetIndex() << std::endl;
		}
	}
	outfile << std::endl;

	//Saves out internal mass data
	outfile << "INTMASSES: " << m_internalMasses.size() << std::endl;
	for (auto massIter = m_internalMasses.begin(); massIter != m_internalMasses.end(); massIter++)
	{
		//Saves out position, mass value and whether mass is fixed
		outfile << "INTMASS " << (*massIter)->GetIndex() << ": " << std::endl;
		outfile << "	Pos: " << std::fixed << (*massIter)->GetOrignalPos().x << " " << (*massIter)->GetOrignalPos().y << " " << (*massIter)->GetOrignalPos().z << std::endl;
		outfile << "	Mass: " << std::fixed << (*massIter)->GetMass() << std::endl;
		outfile << "	Fixed: " << std::fixed << (int)(*massIter)->GetFixed() << std::endl;

		//Saves out structural neighbour indices
		outfile << "	Neighbours: " << std::fixed << (*massIter)->GetNumberOfNeighbours() << std::endl;
		outfile << "		";
		for (auto neighbourIter = (*massIter)->GetNeighboursBeginning(); neighbourIter != (*massIter)->GetNeighbourEnd(); neighbourIter++)
		{
			outfile << std::fixed << (*neighbourIter)->GetIndex() << " ";
		}
		outfile << std::endl;

		//Saves out opposing mass index
		MSMass* opposingMass = (*massIter)->GetOpposingMass();
		if (opposingMass)
		{
			outfile << "	Opposing: " << std::fixed << opposingMass->GetIndex() << std::endl;
		}
	}
	outfile << std::endl;

	//Saves out triangle data
	outfile << "TRIANGLES: " << m_triangles.size() << std::endl;
	for (auto triangleIter = m_triangles.begin(); triangleIter != m_triangles.end(); triangleIter++)
	{
		outfile << (*triangleIter)->m_index << ": " << std::fixed << (*triangleIter)->m_massA << " " << (*triangleIter)->m_massB << " " << (*triangleIter)->m_massC << std::endl;
	}
	outfile << std::endl;

	//Saves out spring data
	outfile << "SPRINGS: " << m_springs.size() << std::endl;
	for (auto springIter = m_springs.begin(); springIter != m_springs.end(); springIter++)
	{
		outfile << (*springIter)->GetIndex() << ": " << std::fixed << (*springIter)->GetMassA()->GetIndex() << " " << (*springIter)->GetMassB()->GetIndex() << std::endl;
	}
	outfile << std::endl;

	outfile.close();

}