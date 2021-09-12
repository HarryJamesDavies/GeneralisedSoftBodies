//=================================================================
// Managers the user interface and interactions
//=================================================================

#ifndef _GUI_MANAGER_H_
#define _GUI_MANAGER_H_
#include <windows.h>
#include "Effects.h"
#include <AntTweakBar.h>

using namespace std;
using namespace DirectX;

struct GlobalData;

class GUIManager
{
public:
	GUIManager();
	~GUIManager();

	void Initialise(GlobalData* _GD);

	void Update();

private:
	void CreateMSOManagerBar();
	void CreatePhysicsBar();

	void CreateFileBar();
	void CreateFBXBar();

	void CreateChainBar();
	void CreateSquareBar();
	void CreateCubeBar();

	//Tool bars
	TwBar *msoManagerBar;
	TwBar *msoBar;
	TwBar *physicsBar;

	GlobalData* m_GD;

	//GUI flags
	bool m_pauseOnSpawn = false;

	bool m_createFile = false;
	bool m_spawnFile = false;

	bool m_createFBX = false;
	bool m_spawnFBX = false;

	bool m_createPrimitive = false;
	bool m_spawnPrimitive = false;

	bool m_saveMSO = false;

	char* m_fileName;
};

#endif;
