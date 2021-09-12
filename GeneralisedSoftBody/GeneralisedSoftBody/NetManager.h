//==============================================================
//Used in basic spring tests as mass/spring model manager
//==============================================================

#pragma once
#include <list>
#include "gameobject.h"

#include "NetAnchor.h"

class NetManager : public GameObject
{
public:
	NetManager(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, GlobalData* _GD);
	virtual ~NetManager();

	void InitNet(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, GlobalData* _GD);

	virtual void Tick(GlobalData* _GD) { return; };
	virtual void Draw(DrawData3D* _DD) { return; };

private:
	std::vector<NetAnchor*> m_netAnchors;

};
