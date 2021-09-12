//=================================================================
// Vertex based Cube object
//=================================================================

#ifndef _VB_BOX_H_
#define _VB_BOX_H_
#include "VertexGO.h"
#include "Vertex.h"

class VertexBox : public VertexGO
{
public:
	VertexBox(int _size, ID3D11Device* _ID, GlobalData* _GD);
	virtual ~VertexBox();

	void init(int _size, ID3D11Device* _ID, GlobalData* _GD);

protected:
	//Dimensions of individual sections
	int m_size;

	int m_numVerts;
	WORD* m_indices;
};

#endif