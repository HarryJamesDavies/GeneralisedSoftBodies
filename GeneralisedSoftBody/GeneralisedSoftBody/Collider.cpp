//=================================================================
// Base collider from which all other colliders inherit
//=================================================================

#include "Collider.h"
#include "GlobalData.h"

Collider::Collider(GameObject* _parent, GlobalData* _GD)
{
	m_parent = _parent;
	m_pos = _parent->GetPos();

	//Adds to list of callable colliders
	m_GD = _GD;
	m_GD->m_physicsManager->AddCollider(this);
}

Collider::~Collider()
{
	m_parent = nullptr;

	//Remove colldier from callable list
	m_GD->m_physicsManager->RemoveCollider(this);
	m_GD = nullptr;
}