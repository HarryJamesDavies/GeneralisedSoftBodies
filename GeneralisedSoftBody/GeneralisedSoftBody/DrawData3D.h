//=================================================================
//Data to be passed by game to all Game Objects via Draw 
//=================================================================

#ifndef _DRAW_STATE_3D_H_
#define _DRAW_STATE_3D_H_
#include "CommonStates.h"

using namespace DirectX;

class Camera;
class Light;

struct DrawData3D
{
	ID3D11DeviceContext* m_pd3dImmediateContext;
	CommonStates* m_states;
	Camera* m_cam;
	Light* m_light;
};

#endif
