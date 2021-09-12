//=================================================================
// Contains set of three masses used to construct mass triangle
//=================================================================

#include "MSTriangle.h"

//Check whether triangle contains both _massA and _massB.
bool MSTriangle::ContainMasses(int _massA, int _massB)
{
	if (ContainMass(_massA) && ContainMass(_massB))
	{
		return true;
	}
	return false;
}

//Determines whether the triangle contains _mass.
bool MSTriangle::ContainMass(int _mass)
{
	if (m_massA != _mass)
	{
		if (m_massB != _mass)
		{
			if (m_massC == _mass)
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	else
	{
		return true;
	}

	return false;
}

//Returns the one mass not passed as a parameter.
int MSTriangle::GetOtherMass(int _massA, int _massB)
{
	if (m_massA != _massA && m_massA != _massB)
	{
		return m_massA;
	}

	if (m_massB != _massA && m_massB != _massB)
	{
		return m_massB;
	}

	if (m_massC != _massA && m_massC != _massB)
	{
		return m_massC;
	}

	return -1;
}