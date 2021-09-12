//=================================================================
// Mass point used to bind vertices and adust thier position based on applied forces
//=================================================================

#include "MSMass.h"
#include "MSSpring.h"
#include "MSMesh.h"
#include "VertexMSO.h"

MSMass::MSMass(int _massIndex, float _mass, Vector3 _pos, MSMesh* _mesh, bool _fixed, bool _exteriorForces)
{
	m_index = _massIndex;
	m_mass = _mass;
	m_orignalPos = _pos;
	m_pos = _pos;
	m_mesh = _mesh;
	m_fixed = _fixed;
	m_exteriorForces = _exteriorForces;

	m_velocity = Vector3::Zero;
	m_oldVelocity = Vector3::Zero;
}

MSMass::~MSMass()
{
	m_mesh = nullptr;
	m_boundVertices.clear();
}

//Calculates the displacement of bound vertices based on applied forces
void MSMass::CalculateDisplacement(GlobalData* _GD)
{
	Vector3 overallAcceleration = m_velocity;
	m_oldVelocity = m_velocity;

	//Prevents physics application if mass is fixed in place
	if (!m_fixed)
	{
		//Applies external forces when true
		if (m_exteriorForces)
		{
			overallAcceleration += _GD->m_physicsManager->GetGravityForce();
			overallAcceleration += _GD->m_physicsManager->GetWindForce() / m_mass;
			overallAcceleration += _GD->m_physicsManager->CalculateColliderForce(m_pos, m_oldVelocity);

			//Applies pressure if the mesh has internal space
			if (m_mesh->Get3D())
			{
				overallAcceleration += _GD->m_physicsManager->m_physicsData.m_pressureCoefficient * m_normal;
			}
		}

		//Adds pre-calculated spring force
		overallAcceleration += m_springForce / m_mass;

		//Converts acceleration to velocity adn minus drag
		m_velocity = (m_oldVelocity + (overallAcceleration *_GD->m_dt)) - _GD->m_physicsManager->m_physicsData.m_dragCoefficient * m_velocity;

		//Limits velocity based on user defined upper velocity limit
		if (m_velocity.Length() > _GD->m_physicsManager->m_physicsData.m_velocityLimit)
		{
			m_velocity.Normalize();
			m_velocity *= _GD->m_physicsManager->m_physicsData.m_velocityLimit;
		}

		//Calculates displacement relative to velocity
		m_displacement = m_velocity * _GD->m_dt;
	}
}

//Applies vertex displacement based on previous calculation
void MSMass::ApplyDisplacement()
{
	//Applies displacement to mass
	m_pos = m_pos + m_displacement;

	//Applies displacement to bound vertices
	for (auto it = m_boundVertices.begin(); it != m_boundVertices.end(); it++)
	{
		m_mesh->GetParent()->SetVertexPos((*it), (m_mesh->GetParent()->GetVertexPos((*it)) + m_displacement));
	}
}

//Binds vertex to mass, allowing the application of displacement
void MSMass::BindVertex(int _vertexIndex)
{
	m_boundVertices.push_back(_vertexIndex);
	return;
}

//Unbinds vertex to mass, unallowing the application of displacement
void MSMass::UnbindVertex(int _vertexIndex)
{
	list<int>::iterator iter;

	for (auto it = m_boundVertices.begin(); it != m_boundVertices.end(); it++)
	{
		if ((*it) == _vertexIndex)
		{
			iter = it;
		}
	}

	m_boundVertices.remove((*iter));
}