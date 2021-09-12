//=================================================================
// Stores, draws, ticks and deletes all objects
//=================================================================

#include "GameObjectPool.h"
#include "GameObject.h"
#include "GlobalData.h"
#include "ObjectLayer.h"

GameObjectPool::GameObjectPool()
{
	//Adds a new list of game objects per layer
	m_GameObjects.insert(ListLayer(ObjectLayer::OL_NULL, new ObjectList));
	m_GameObjects.insert(ListLayer(ObjectLayer::OL_CAMERA, new ObjectList));
	m_GameObjects.insert(ListLayer(ObjectLayer::OL_LIGHT, new ObjectList));
}

GameObjectPool::~GameObjectPool()
{
	//Deletes all 2D game objects
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		for (auto listIt = it->second->begin(); listIt != it->second->end(); listIt++)
		{
			delete(*listIt);
		}
		it->second->clear();
	}
	m_GameObjects.clear();
}

//Gets a specific object by passing in the object's name
GameObject* GameObjectPool::GetGameObject(std::string _objectName, ObjectLayer _layer)
{
	for (auto it = GetObjectListBegin(_layer); it != GetObjectListEnd(_layer); it++)
	{
		if ((*it)->GetName() == _objectName)
		{
			return (*it);
		}
	}
	return nullptr;
}

//Stores an object on the relative layer, specify in the objects constructor
void GameObjectPool::StoreGameObject(GameObject * _gameObject)
{
	ObjectLayer layer = _gameObject->GetLayer();
	m_GameObjects.find(layer)->second->push_back(_gameObject);
	return;
}

//Ticks specific object by name
void GameObjectPool::TickObject(GlobalData* _GD, std::string _objectName)
{
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		for (auto listIt = it->second->begin(); listIt != it->second->end(); listIt++)
		{
			if ((*listIt)->GetName() == _objectName)
			{
				(*listIt)->Tick(_GD);
			}
		}
	}
}

//Ticks specific layer of objects
void GameObjectPool::TickLayer(GlobalData* _GD, ObjectLayer _layer)
{
	for (auto it = GetObjectListBegin(_layer); it != GetObjectListEnd(_layer); it++)
	{
		(*it)->Tick(_GD);
	}
}

//Ticks all game objects
void GameObjectPool::Tick(GlobalData* _GD)
{
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		for (auto listIt = it->second->begin(); listIt != it->second->end(); listIt++)
		{
			(*listIt)->Tick(_GD);
		}
	}
}

//Draws a specific object by name
void GameObjectPool::DrawObject(DrawData3D* _DD, std::string _objectName)
{
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		for (auto listIt = it->second->begin(); listIt != it->second->end(); listIt++)
		{
			if ((*listIt)->GetName() == _objectName)
			{
				(*listIt)->Draw(_DD);
			}
		}
	}
}

//Draws all objects in a specifc layer
void GameObjectPool::DrawLayer(DrawData3D* _DD, ObjectLayer _layer)
{
	for (auto it = GetObjectListBegin(_layer); it != GetObjectListEnd(_layer); it++)
	{
		(*it)->Draw(_DD);
	}
}

//Draws all game objects
void GameObjectPool::Draw(DrawData3D* _DD)
{
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		for (auto listIt = it->second->begin(); listIt != it->second->end(); listIt++)
		{
			(*listIt)->Draw(_DD);
		}
	}
}

//Sets a specific object to drawable
void GameObjectPool::SetDrawable(bool _drawable, std::string _objectName, ObjectLayer _layer)
{
	for (auto it = GetObjectListBegin(_layer); it != GetObjectListEnd(_layer); it++)
	{
		if ((*it)->GetName() == _objectName)
		{
			(*it)->SetDrawable(_drawable);
		}
	}
}

//Deletes specific object from object pool
void GameObjectPool::DeleteObject(std::string _objectName)
{
	GameObject* objectToRemove = nullptr;
	ObjectLayer layer = ObjectLayer::OL_NULL;

	//Gets a specific object to be deleted
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		for (auto listIt = it->second->begin(); listIt != it->second->end(); listIt++)
		{
			if ((*listIt)->GetName() == _objectName)
			{
				layer = it->first;
				objectToRemove = (*listIt);
			}
		}
	}

	//Deletes specific object
	if (objectToRemove != nullptr)
	{
		m_GameObjects.find(layer)->second->remove(objectToRemove);
	}
	objectToRemove = nullptr;
}

//Deletes a layer of objects
void GameObjectPool::DeleteLayer(ObjectLayer _layer)
{
	for (auto it = GetObjectListBegin(_layer); it != GetObjectListEnd(_layer); it++)
	{
		delete (*it);
	}
	m_GameObjects.find(_layer)->second->clear();

	return;
}

//Deletes all objects
void GameObjectPool::Delete()
{
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		for (auto listIt = it->second->begin(); listIt != it->second->end(); listIt++)
		{
			delete (*listIt);
		}
		it->second->clear();
	}
}