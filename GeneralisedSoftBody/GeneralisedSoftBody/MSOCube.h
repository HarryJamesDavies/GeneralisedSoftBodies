//=================================================================
// Mass/spring cube primitive
//=================================================================

#ifndef _MSO_CUBE_H_
#define _MSO_CUBE_H_
#include "VertexMSO.h"
#include <list>

class MSOCube: public VertexMSO
{
public:
	MSOCube(int _width, int _height, int _depth, Vector3 _numSections, bool _generateInternals, GlobalData* _GD);
	virtual ~MSOCube();

	void Initilaise(bool _generateInternals, ID3D11Device* _ID);

	virtual void Tick(GlobalData* _GD) { VertexMSO::Tick(_GD); };
	virtual void Draw(DrawData3D* _DD) { VertexMSO::Draw(_DD); };

private:
	void CreateVertices();

	//Dimensions of individual sections
	int m_width;
	int m_height;
	int m_depth;

	Vector3 m_numSections;
};

#endif;