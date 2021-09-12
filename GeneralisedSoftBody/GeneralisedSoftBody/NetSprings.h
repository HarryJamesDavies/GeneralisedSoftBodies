//==============================================================
//Used in basic spring tests to calculate spring force
//==============================================================

#pragma once
#include "d3d11_1.h"
#include "SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class NetAnchor;
struct GlobalData;

class NetSpring
{
public:
	NetSpring(NetAnchor* _anchorA, NetAnchor* _anchorB);
	~NetSpring();

	Vector3 CalculateForce(GlobalData* _GD);

private:
	NetAnchor* m_anchorA;
	NetAnchor* m_anchorB;
	Vector3 m_equilibriumVector;
	float m_equilibriumDistance;

	const float MAXFORCE = 10.0f;
};
