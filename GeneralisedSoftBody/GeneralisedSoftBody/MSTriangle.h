//=================================================================
// Contains set of three masses used to construct mass triangle
//=================================================================

#ifndef _MS_TRIANGLE_H_
#define _MS_TRIANGLE_H_

class MSEdge;

class MSTriangle
{
public:
	MSTriangle() {};
	~MSTriangle() {};

	int m_index;

	//Mass indices
	int m_massA;
	int m_massB;
	int m_massC;

	MSEdge* m_edgeA;
	MSEdge* m_edgeB;
	MSEdge* m_edgeC;

	bool ContainMasses(int _massA, int _massB);
	bool ContainMass(int _mass);

	int GetOtherMass(int _massA, int _massB);

private:


};

#endif;
