//=================================================================
// Base collider from which all other colliders inherit
//=================================================================

#ifndef _COLLIDER_H_
#define _COLLIDER_H_
#include "d3d11_1.h"
#include "SimpleMath.h"
#include "GameObject.h"

struct GlobalData;

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Collider
{
public:
	Collider(GameObject* _parent, GlobalData* _GD);
	virtual ~Collider();

	virtual Vector3 CalculateForce(Vector3 _pos, Vector3 _otherVelocity) = 0;
	void UpdatePosition() { m_pos = m_parent->GetPos(); };

protected:
	Vector3 m_pos;
	GameObject* m_parent;
	GlobalData* m_GD;
};

#endif
