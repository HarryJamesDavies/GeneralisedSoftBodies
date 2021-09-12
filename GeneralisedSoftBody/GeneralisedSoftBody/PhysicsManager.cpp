//=================================================================
// Manages all global interactions and updates physics systems
//=================================================================

#include "PhysicsManager.h"
#include <math.h>

PhysicsManager::PhysicsManager()
{
	m_sinCount = 0.0f;
	m_windPercentage = 0.0f;
	m_colliders.clear();
}

PhysicsManager::~PhysicsManager()
{

}

//Returns gravity force relative to gravity coefficient
Vector3 PhysicsManager::GetGravityForce()
{
	Vector3 gravityForce = m_physicsData.m_gravityCoefficient * Vector3(0.0f, 10.0f, 0.0f);
	return gravityForce;
}

//Returns wind force relative to wind coefficient
Vector3 PhysicsManager::GetWindForce()
{
	Vector3 normWindDir = m_physicsData.m_windDirection;
	Vector3 windForce = m_physicsData.m_windCoefficient * normWindDir;
	return windForce;
}

//Update all physics systems each frame
void PhysicsManager::UpdatePhysics(float _dt)
{
	UpdateWindCoefficent(_dt);
	UpdateColliders();
}

//Alternates wind force between maximum and minimum
void PhysicsManager::UpdateWindCoefficent(float _dt)
{
	//Gets the difference between max and minimum and calculates a percentage using a sin wave and delta time
	m_sinCount += _dt;

	float windDif = m_physicsData.m_windCoefficientMax - m_physicsData.m_windCoefficientMin;

	m_windPercentage = sin(m_sinCount);

	windDif *= m_windPercentage;

	m_physicsData.m_windCoefficient = m_physicsData.m_windCoefficientMin + windDif;
}

//Update all colliders to their new position each frame
void PhysicsManager::UpdateColliders()
{
	for (auto it = m_colliders.begin(); it != m_colliders.end(); it++)
	{
		(*it)->UpdatePosition();
	}
}

//Calculates collsion force relative to an objects position in scene
Vector3 PhysicsManager::CalculateColliderForce(Vector3 _position, Vector3 _otherVelocity)
{
	Vector3 force = Vector3::Zero;

	for (auto it = m_colliders.begin(); it != m_colliders.end(); it++)
	{
		force += (*it)->CalculateForce(_position, _otherVelocity);
	}

	return force;
}

//Adds a colldier to collider call list 
void PhysicsManager::AddCollider(Collider* _collider)
{
	m_colliders.push_back(_collider);
}

//Removes a colldier to collider call list 
void PhysicsManager::RemoveCollider(Collider* _collider)
{
	m_colliders.remove(_collider);
}