//=================================================================
// Stores, draws, ticks and deletes all objects
//=================================================================

#pragma once
#include <list>
#include <map>
#include "ObjectLayer.h"

class GameObject;
struct GlobalData;
struct DrawData3D;

using ObjectList = std::list<GameObject*>;
using ListLayer = std::pair<ObjectLayer, ObjectList*>;

class GameObjectPool
{
public:
	GameObjectPool();
	~GameObjectPool();

	std::list<GameObject*>* GetObjectList(ObjectLayer _layer) { return m_GameObjects.find(_layer)->second; };
	std::list<GameObject*>::iterator GetObjectListBegin(ObjectLayer _layer) { return m_GameObjects.find(_layer)->second->begin(); };
	std::list<GameObject*>::iterator GetObjectListEnd(ObjectLayer _layer) { return m_GameObjects.find(_layer)->second->end(); };

	GameObject* GetGameObject(std::string _objectName, ObjectLayer _layer);
	void StoreGameObject(GameObject* _gameObject);

	void TickObject(GlobalData* _GD, std::string _objectName);
	void TickLayer(GlobalData* _GD, ObjectLayer _layer);
	void Tick(GlobalData* _GD);

	void DrawObject(DrawData3D* _DD, std::string _objectName);
	void DrawLayer(DrawData3D* _DD, ObjectLayer _layer);
	void Draw(DrawData3D* _DD);
	void SetDrawable(bool _drawable, std::string _objectName, ObjectLayer _layer);

	void DeleteObject(std::string _objectName);
	void DeleteLayer(ObjectLayer _layer);
	void Delete();

private:
	//All game objects
	std::map<ObjectLayer, std::list<GameObject*>*> m_GameObjects;
};