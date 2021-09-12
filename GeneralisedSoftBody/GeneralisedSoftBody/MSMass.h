//=================================================================
// Mass point used to bind vertices and adust thier position based on applied forces
//=================================================================

#ifndef _M_S_MASS_H_
#define _M_S_MASS_H_
#include "d3d11_1.h"
#include "SimpleMath.h"
#include <list>
#include "GlobalData.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace std;

class MSMesh;
class MSSpring;

class MSMass
{
public:
	MSMass(int _massIndex, float _mass, Vector3 _pos, MSMesh* _mesh, bool _fixed, bool _exteriorForces);
	~MSMass();

	void CalculateDisplacement(GlobalData* _GD);

	void ApplyDisplacement();

	Vector3 GetNormal() { return m_normal; };
	void SetNormal(Vector3 _normal) { m_normal = _normal; m_normal.Normalize(); };
	void AddNormal(Vector3 _normal) { m_normal += _normal; };

	void BindVertex(int _vertexIndex);
	void UnbindVertex(int _vertexIndex);

	int GetIndex() { return m_index; };
	void SetIndex(int _index) { m_index = _index; };

	Vector3 GetDisplacement() { return m_displacement; };
	void SetDisplacement(Vector3 _displacement) { m_displacement = _displacement; };

	Vector3 GetPos() { return m_pos; };
	void SetPos(Vector3 _pos) { m_pos = _pos; };
	Vector3 GetOrignalPos() { return m_orignalPos; };

	float GetMass() { return m_mass; };

	bool GetFixed() { return m_fixed; };
	bool GetExterior() { return m_exteriorForces; };

	void SetSpringForce(Vector3 _springForce) { m_springForce = _springForce; };
	void AddSpringForce(Vector3 _springForce) { m_springForce += _springForce; };

	void SetPressureForce(float _pressureForce) { m_pressureForce = _pressureForce; };
	void AddPressureForce(float _pressureForce) { m_pressureForce += _pressureForce; };

	Vector3 GetVelocity() { return m_velocity; };
	void SetVelocity(Vector3 _velocity) { m_velocity = _velocity; };

	int GetNumberOfVertices() { return m_boundVertices.size(); };
	list<int>::iterator GetVerticesBegin() { return m_boundVertices.begin(); };
	list<int>::iterator GetVerticesEnd() { return m_boundVertices.end(); };

	void AddNeighbour(MSMass* _neighbour) { m_structuralNeighbours.push_back(_neighbour); };
	int GetNumberOfNeighbours() { return m_structuralNeighbours.size(); };
	list<MSMass*>::iterator GetNeighboursBeginning() { return m_structuralNeighbours.begin(); };
	list<MSMass*>::iterator GetNeighbourEnd() { return m_structuralNeighbours.end(); };

	void AssignOpposingMass(MSMass* _opposingMass) { m_opposingMass = _opposingMass; };
	MSMass* GetOpposingMass() { return m_opposingMass; };

private:
	//Vertices directly adjusted by this mass
	list<int> m_boundVertices;

	//The mass on the opposing layer directly connected to this one
	MSMass* m_opposingMass = nullptr;

	//Neighbours directly attached to this mass via structural springs
	list<MSMass*> m_structuralNeighbours;

	int m_index;

	float m_mass;

	Vector3 m_orignalPos;
	Vector3 m_pos;
	Vector3 m_displacement;
	Vector3 m_velocity;
	Vector3 m_oldVelocity;
	Vector3 m_normal;

	Vector3 m_springForce;
	float m_pressureForce;

	MSMesh* m_mesh;

	//Flag determines whether the mass is fixed in place
	bool m_fixed;
	bool m_exteriorForces = true;
};

#endif
