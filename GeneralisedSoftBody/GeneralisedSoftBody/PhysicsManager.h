//=================================================================
// Manages all global interactions and updates physics systems
//=================================================================

#ifndef _PHYSIC_MANAGER_H_
#define _PHYSIC_MANAGER_H_
#include <list>
#include "PhysicsData.h"
#include "Collider.h"

using namespace std;

class PhysicsManager
{
public:
	PhysicsManager();
	~PhysicsManager();

	void UpdatePhysics(float _dt);

	Vector3 GetGravityForce();
	Vector3 GetWindForce();

	PhysicsData m_physicsData;

	Vector3 CalculateColliderForce(Vector3 _position, Vector3 _otherVelocity);
	void AddCollider(Collider* _collider);
	void RemoveCollider(Collider* _collider);

private:	
	void UpdateColliders();
	void UpdateWindCoefficent(float _dt);

	float m_windPercentage;
	float m_sinCount;

	list<Collider*> m_colliders;
};

#endif 


