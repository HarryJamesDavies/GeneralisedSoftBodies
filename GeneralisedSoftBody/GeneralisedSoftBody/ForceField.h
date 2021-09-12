//=================================================================
// Omni-directional collider that only apllies force
//=================================================================

#pragma once
#include "Collider.h"

struct GlobalData;

class ForceField : public Collider
{
public:
	ForceField(GameObject* _parent, GlobalData* _GD, float _fieldStrength, float _minimumDistance);
	virtual ~ForceField();

	virtual Vector3 CalculateForce(Vector3 _otherPosition, Vector3 _otherVelocity);

private:
	float m_fieldStrength;
	float m_minimumDistanceSqrd;
};