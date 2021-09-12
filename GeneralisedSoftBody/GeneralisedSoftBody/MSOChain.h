//=================================================================
// Base class for mass/spring chains
//=================================================================

#ifndef _MSO_CHAIN_H_
#define _MSO_CHAIN_H_
#include "VertexMSO.h"
#include <list>

class MSOChain: public VertexMSO
{
public:
	MSOChain(int _width, int _height, int _numSections, bool _generateInternals, GlobalData* _GD);
	virtual ~MSOChain();

	//Initialise the Vertex and Index buffers for the cube
	void Initilaise(bool _generateInternals, ID3D11Device* _ID);

	virtual void Tick(GlobalData* _GD) { VertexMSO::Tick(_GD); };
	virtual void Draw(DrawData3D* _DD) { VertexMSO::Draw(_DD); };

private:
	void CreateVertices();

	//Dimensions of individual sections
	int m_width;
	int m_height;

	int m_numSections;
};

#endif;