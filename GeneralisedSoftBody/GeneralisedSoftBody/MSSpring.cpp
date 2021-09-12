//=================================================================
// Spring used to calculate the resistance force when compressed or stretched between masses
//=================================================================

#include "MSSpring.h"
#include "MSMass.h"

MSSpring::MSSpring(MSMass* _massA, MSMass* _massB, int _index)
{
	m_massA = _massA;
	m_massB = _massB;

	//Sets equilibrium distance and vector
	m_equilibriumVector = m_massB->GetPos() - m_massA->GetPos();
	m_equilibriumDistance = Vector3::Distance(m_massB->GetPos(), m_massA->GetPos()); // il
	m_equilibriumVector.Normalize();

	m_index = _index;
}

MSSpring::~MSSpring()
{
	m_massA = nullptr;
	m_massB = nullptr;
}

void MSSpring::CalculateForce(GlobalData* _GD)
{
	//Calculates spring force using hooke's law	
	m_massDistance = Vector3::Distance(m_massB->GetPos(), m_massA->GetPos()); // dl
	float force = _GD->m_physicsManager->m_physicsData.m_springCoefficient * (m_massDistance - m_equilibriumDistance); // f = k * (dl - il)

	//Calculate damping force combined with Hooke's law
	Vector3 massVector = m_massB->GetPos() - m_massA->GetPos(); // r
	massVector.Normalize();

	Vector3 velocityVector = m_massB->GetVelocity() - m_massA->GetVelocity(); // vr

	Vector3 forceVector = (massVector * force) + (_GD->m_physicsManager->m_physicsData.m_dampingCoefficient * (velocityVector * massVector)) * massVector; // F = (r * f) + (d *(vr * r)) * r

	//Adds force to both masses
	m_massA->AddSpringForce(forceVector);
	m_massB->AddSpringForce(-forceVector);
}

//Sets the pressure value of the masses connected to spring
void MSSpring::SetMassPressure(float _pressure)
{
	m_massA->AddPressureForce(_pressure);
	m_massB->AddPressureForce(_pressure);
}

//Checks whether spring is attached to _mass
bool MSSpring::ContainsMass(MSMass* _mass)
{
	if (m_massA == _mass || m_massB == _mass)
	{
		return true;
	}
	return false;
}