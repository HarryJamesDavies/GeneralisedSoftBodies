//=================================================================
// Base class for all model based game objects
//=================================================================

#ifndef _CMOGO_H_
#define _CMOGO_H_
#include "gameobject.h"
#include "Model.h"
#include <string>

using namespace std;
using namespace DirectX;

class MeshGO : public GameObject
{
public:
	MeshGO(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF);
	virtual ~MeshGO();

	virtual void Tick(GlobalData* _GD);
	virtual void Draw(DrawData3D* _DD);

protected:
	unique_ptr<Model>  m_model;

	//needs a slightly different raster state that the VBGOs so create one and let them all use it
	static ID3D11RasterizerState*  s_pRasterState;
	static int m_count;
};

#endif
