//=================================================================
// Contains a pair of masses used to construct mass triangle
//=================================================================

#ifndef _MS_EDGE_H_
#define _MS_EDGE_H_

class MSTriangle;

class MSEdge
{
public:
	MSEdge() {};
	~MSEdge() {};

	int m_index;

	MSTriangle* m_triangle;

	int m_massAIndex;
	int m_massBIndex;

private:

};

#endif;
