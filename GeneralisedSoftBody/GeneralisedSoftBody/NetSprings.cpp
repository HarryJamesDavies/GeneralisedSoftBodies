//==============================================================
//Used in basic spring tests to calculate spring force
//==============================================================

#include "NetSprings.h"
#include "GlobalData.h"
#include "NetAnchor.h"


NetSpring::NetSpring(NetAnchor* _anchorA, NetAnchor* _anchorB)
{
	m_anchorA = _anchorA;
	m_anchorB = _anchorB;

	//Sets equilibrium distance and vector
	m_equilibriumVector = m_anchorA->GetPos() - m_anchorB->GetPos();
	m_equilibriumDistance = Vector3::Distance(m_anchorA->GetPos(), m_anchorB->GetPos());
	m_equilibriumVector.Normalize();
}

NetSpring::~NetSpring()
{

}

//Calculates spring force using Hooke's law
Vector3 NetSpring::CalculateForce(GlobalData* _GD)
{
	//Calculates spring force using hooke's law
	Vector3 anchorVector = m_anchorA->GetPos() - m_anchorB->GetPos();
	float anchorDistance = Vector3::Distance(m_anchorA->GetPos(), m_anchorB->GetPos());
	anchorVector.Normalize();
	return -1.0f *  _GD->m_physicsManager->m_physicsData.m_springCoefficient * (anchorDistance - m_equilibriumDistance) * anchorVector;
}
