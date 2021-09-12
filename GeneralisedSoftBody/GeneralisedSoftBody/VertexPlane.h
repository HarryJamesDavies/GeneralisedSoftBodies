//=================================================================
// Vertex based plane object
//=================================================================

#pragma once
#ifndef _VB_PLANE_H_
#define _VB_PLANE_H_
#include "VertexGO.h"
#include "Vertex.h"
#include <list>

struct GlobalData;
struct DrawData3D;

class VertexPlane : public VertexGO
{
public:
	VertexPlane(int _width, int _height, Vector2 _numSections, ID3D11Device* _ID, GlobalData* _GD);
	virtual ~VertexPlane();

	void init(ID3D11Device* _ID);

	virtual void Tick(GlobalData* _GD);

private:

	void CalculateNormals();
	void CreateVertices();

	//Dimensions of individual sections
	int m_width;
	int m_height;

	int m_size;

	Vector2 m_numSections;
};

#endif