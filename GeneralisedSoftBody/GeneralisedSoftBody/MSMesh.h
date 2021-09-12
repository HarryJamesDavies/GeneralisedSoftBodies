//=================================================================
// Combined mesh of masses and springs
//=================================================================

#ifndef _M_S_MESH_H_
#define _M_S_MESH_H_
#include <list>
#include "d3d11_1.h"
#include "SimpleMath.h"
#include "FixDimension.h"
#include "FixFormat.h"

using namespace std;

class VertexMSO;
class MSMass;
class MSSpring;
class Triangle;
class Edge;
struct GlobalData;
class MSTriangle;

using namespace DirectX;
using namespace DirectX::SimpleMath;

#define PI = 3.14159265359;

class MSMesh
{
public:
	MSMesh(VertexMSO* _parent, int _fixIndex, FixFormat _format, FixDimension _dimension);
	~MSMesh();

	void CreateMasses1D(int _maxVertices, int _width, int _height, int _numSectionsX);
	void CreateSurfaceMasses(int _maxVertices, int _maxTriangles, std::list<Triangle*>::iterator _trianglesIter, std::list<Triangle*>::iterator _trianglesEnd);
	void CreateInternalMasses();

	void CreateFromFile(const char* _fileName);

	void AssignVertices(MSMass* _mass);

	void ResetMasses();

	void CreateSprings1D(int _numSectionsX);
	void CreateSurfaceSprings();
	void CreateInternalSprings();
	bool CheckExistingSpring(MSMass* _massA, MSMass* _massB);

	void UpdateNormals();

	void UpdateSprings(GlobalData* _GD);
	void UpdateMasses(GlobalData* _GD);
	void ApplyChanges();

	VertexMSO* GetParent() { return m_parent; };
	void SetParent(VertexMSO* _parent) { m_parent = _parent; };

	MSMass* GetMass(int _index);
	MSMass* GetExternalMass(int _index);
	float GetPressure() { return m_pressure; };

	void CalculateNormals(MSTriangle* _triangle);
	float CalculateAngle(Vector3 _angleVertex, Vector3 _nextVertexA, Vector3 _prevVertex);

	bool Get3D() { return m_3D; };
	void Set3D(bool _3D) { m_3D = _3D; };

	list<MSMass*> m_masses;
	list<MSTriangle*> m_triangles;

	void SaveMesh();

	int m_fixIndex;
	FixFormat m_fixFormat;
	FixDimension m_fixDimension;

private:
	VertexMSO* m_parent = nullptr;

	//Vertices to assign to masses
	list<int> m_verticesToAssign;

	float m_volume;
	float m_pressure;

	//List of inner layer masses and springs
	list<MSMass*> m_internalMasses;
	list<MSSpring*> m_springs;

	bool m_3D = false;
};

#endif
