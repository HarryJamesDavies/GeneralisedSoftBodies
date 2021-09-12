//=================================================================
// Base file from which all other systems are created from
//=================================================================

#ifndef _BASE_H_
#define _BASE_H_
#include <windows.h>
#include <memory>
#include "Effects.h"

using namespace std;
using namespace DirectX;

struct GlobalData;

class Base
{
public:
	Base(ID3D11Device* _pd3dDevice, HWND _hWnd, HINSTANCE _hInstance);
	~Base();

	//Core base loops
	bool Update();
	void Render(ID3D11DeviceContext* _pd3dImmediateContext);

private:
	void InitialiseRender(HWND _hWnd);
	void InitialiseUI();
	void InitialisePhysics();
	void InitialisePlayer();

	GlobalData* m_globalData;
};

#endif;