//=================================================================
// Omni-directional collider that only apllies force
//=================================================================

#include "ForceField.h"
#include "GlobalData.h"

ForceField::ForceField(GameObject* _parent, GlobalData* _GD, float _fieldStrength, float _minimumDistance) : Collider(_parent, _GD)
{
	m_fieldStrength = _fieldStrength;
	m_minimumDistanceSqrd = _minimumDistance * _minimumDistance;
}

ForceField::~ForceField()
{
}

//Calculates the amount of force apllied relative the position of opposing object
Vector3 ForceField::CalculateForce(Vector3 _otherPosition, Vector3 _otherVelocity)
{
	//Calculates the distance between force field and opposing object
	float distanceSqrd = Vector3::DistanceSquared(m_pos, _otherPosition);
	float radiusSqrd = m_GD->m_physicsManager->m_physicsData.m_forceFieldRadius * m_GD->m_physicsManager->m_physicsData.m_forceFieldRadius;

	//Ensures force field never drops below minimum
	if (radiusSqrd < m_minimumDistanceSqrd)
	{
		radiusSqrd = m_minimumDistanceSqrd;
	}

	//If with the radius of ofrce field, will aply force along the vector from the collider to the opposing obejct
	if (distanceSqrd <= radiusSqrd)
	{
		Vector3 forceVector = m_pos - _otherPosition;
		return -forceVector * (m_fieldStrength *  m_GD->m_physicsManager->m_physicsData.m_forceFieldCoefficient);
	}
	return Vector3::Zero;
}