//==============================================================
//Used in basic spring tests as mass/spring model manager
//==============================================================

#include "NetManager.h"
#include "GlobalData.h"

NetManager::NetManager(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, GlobalData* _GD) : GameObject()
{
	m_layer = ObjectLayer::OL_NULL;
	InitNet(_fileName, _pd3dDevice, _EF, _GD);
}

NetManager::~NetManager()
{

}

//Initialise test net mass/spring structure
void NetManager::InitNet(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, GlobalData* _GD)
{
	//Creates a line of anchors connected to two fixed anchors
	int i = 1;
	m_netAnchors.push_back(new NetAnchor(_fileName, _pd3dDevice, _EF, Vector3(-10.0f, 0.0f, 0.0f), i++, true, false));
	_GD->m_gameObjectPool->StoreGameObject(m_netAnchors.back());
	m_netAnchors.push_back(new NetAnchor(_fileName, _pd3dDevice, _EF, Vector3(10.0f, 0.0f, 0.0f), i++, true, true));
	_GD->m_gameObjectPool->StoreGameObject(m_netAnchors.back());
	m_netAnchors.push_back(new NetAnchor(_fileName, _pd3dDevice, _EF, Vector3(0.0f, 10.0f, 0.0f), i++, false, false));
	m_netAnchors.back()->AddSpring(m_netAnchors[0]);
	m_netAnchors.back()->AddSpring(m_netAnchors[1]);
	_GD->m_gameObjectPool->StoreGameObject(m_netAnchors.back());
	m_netAnchors.push_back(new NetAnchor(_fileName, _pd3dDevice, _EF, Vector3(0.0f, 0.0f, 0.0f), i++, false, false));
	m_netAnchors.back()->AddSpring(m_netAnchors[2]);
	_GD->m_gameObjectPool->StoreGameObject(m_netAnchors.back());
	m_netAnchors.push_back(new NetAnchor(_fileName, _pd3dDevice, _EF, Vector3(0.0f, -10.0f, 0.0f), i++, false, false));
	m_netAnchors.back()->AddSpring(m_netAnchors[3]);
	m_netAnchors.back()->AddSpring(m_netAnchors[2]);
	_GD->m_gameObjectPool->StoreGameObject(m_netAnchors.back());
	m_netAnchors.push_back(new NetAnchor(_fileName, _pd3dDevice, _EF, Vector3(0.0f, -20.0f, 0.0f), i++, false, false));
	m_netAnchors.back()->AddSpring(m_netAnchors[4]);
	m_netAnchors.back()->AddSpring(m_netAnchors[3]);
	_GD->m_gameObjectPool->StoreGameObject(m_netAnchors.back());
	m_netAnchors.push_back(new NetAnchor(_fileName, _pd3dDevice, _EF, Vector3(0.0f, -30.0f, 0.0f), i++, false, false));
	m_netAnchors.back()->AddSpring(m_netAnchors[5]);
	m_netAnchors.back()->AddSpring(m_netAnchors[4]);
	_GD->m_gameObjectPool->StoreGameObject(m_netAnchors.back());

	return;
}