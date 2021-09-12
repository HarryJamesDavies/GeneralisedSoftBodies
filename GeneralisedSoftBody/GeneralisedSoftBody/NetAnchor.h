//==============================================================
//Used in basic spring tests as masses
//==============================================================

#pragma once
#include "MeshGO.h"
#include <list>

#include "NetSprings.h"

class NetAnchor : public MeshGO
{
public:
	NetAnchor(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, Vector3 _pos, int _anchorNum, bool _fixed, bool _move);
	~NetAnchor();

	virtual void Tick(GlobalData* _GD);

	void AddSpring(NetAnchor* _anchorB);
private:
	int m_anchorNum;
	bool m_fixed;
	bool m_move;

	Vector3 m_velocity = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 m_oldVelocity = Vector3(0.0f, 0.0f, 0.0f);

	std::list<NetSpring*> m_springs;

	Vector2 m_prevMousePos;
	bool m_firstFrame;
};