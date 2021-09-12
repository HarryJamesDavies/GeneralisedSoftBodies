//=================================================================
// Spring used to calculate the resistance force when compressed or stretched between masses
//=================================================================

#ifndef _M_S_SPRING_H_
#define _M_S_SPRING_H_
#include "d3d11_1.h"
#include "SimpleMath.h"

#include "GlobalData.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class MSMass;


class MSSpring
{
public:
	MSSpring(MSMass* _massA, MSMass* _massB, int _index);
	~MSSpring();

	void CalculateForce(GlobalData* _GD);
	void CalculateNormal();

	int GetIndex() { return m_index; };
	void SetIndex(int _index) { m_index = _index; };

	MSMass* GetMassA() { return m_massA; };
	MSMass* GetMassB() { return m_massB; };

	void SetMassPressure(float _pressure);

	bool ContainsMass(MSMass* _mass);

private:
	MSMass* m_massA;
	MSMass* m_massB;

	Vector3 m_equilibriumVector;
	float m_equilibriumDistance;

	float m_massDistance;
	float m_force;

	int m_index;
};

#endif 

