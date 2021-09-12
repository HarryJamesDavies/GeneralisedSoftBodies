//=================================================================
// Collection of all physic coeffiects and parameters
//=================================================================

#ifndef _PHYSICS_DATA_H_
#define _PHYSICS_DATA_H_
#include "d3d11_1.h"
#include "SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

struct PhysicsData 
{
	float m_gravityCoefficient;
	float m_dragCoefficient;
	float m_springCoefficient;
	float m_dampingCoefficient;
	float m_windCoefficient;
	float m_windCoefficientMin;
	float m_windCoefficientMax;
	float m_pressureCoefficient;
	float m_forceFieldCoefficient;
	float m_forceFieldRadius;
	Vector3 m_windDirection;
	float m_velocityLimit;
};

#endif;