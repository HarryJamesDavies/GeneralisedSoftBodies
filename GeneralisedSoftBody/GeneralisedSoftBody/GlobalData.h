#ifndef _GLOBAL_DATA_H_
#define _GLOBAL_DATA_H_
#include <windows.h>
#include <memory>
#include "Effects.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "CommonStates.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "GameObjectPool.h"
#include "PhysicsManager.h"
#include "DrawData2D.h"
#include "DrawData3D.h"
#include "MSOManager.h"
#include "GUIManager.h"

using std::unique_ptr;

//=================================================================
// Any data pssed from system to system is stored here
//=================================================================

struct GlobalData
{
	GlobalData() = default;
	~GlobalData() = default;

	ID3D11Device* m_IDevice;
	ID3D11DeviceContext* m_IDeviceContext;

	//required for the CMO model rendering system
	CommonStates *m_states = nullptr;
	IEffectFactory *m_fxFactory = nullptr;

	////Data to be shared to all 2D Game Objects via Draw
	DrawData2D* m_DD2D;
	//Data to be shared to all 3D Game Objects via Draw 
	DrawData3D* m_DD3D;

	//Input Pointers
	unique_ptr<Keyboard> m_keyboard;
	unique_ptr<Mouse> m_mouse;
	
	//Holds all game objects
	unique_ptr<GameObjectPool> m_gameObjectPool;

	//Window size
	UINT m_width;
	UINT m_height;

	//Time step since last frame
	float m_dt; 

	bool m_pause = false;

	//amount of time since the game started
	DWORD m_playTime;

	//System managers
	unique_ptr<MSOManager> m_MSOManager;
	unique_ptr<GUIManager> m_GUIManager;
	unique_ptr<PhysicsManager> m_physicsManager;
};
#endif;
