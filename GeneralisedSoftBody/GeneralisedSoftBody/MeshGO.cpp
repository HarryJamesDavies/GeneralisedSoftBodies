//=================================================================
// Base class for all model based game objects
//=================================================================

#include "MeshGO.h"
#include <windows.h>
#include "Model.h"
#include "Effects.h"
#include "CommonStates.h"
#include "Helper.h"
#include "Camera.h"
#include "DrawData3D.h"

using namespace DirectX;

ID3D11RasterizerState*  MeshGO::s_pRasterState = nullptr;
int MeshGO::m_count = 0;

MeshGO::MeshGO(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) :m_model(nullptr)
{
	//if we've not created it yet do so now
	if (!s_pRasterState)
	{
		//Setup Raster State
		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		// Create the rasterizer state from the description we just filled out.
		HRESULT hr = _pd3dDevice->CreateRasterizerState(&rasterDesc, &s_pRasterState);
	}

	string filePath =
#ifdef DEBUG
		"../Debug/"
#else
		"../Release/"
#endif
		+ _fileName;

	wchar_t* file = Helper::charToWChar(filePath.c_str());

	m_model = Model::CreateFromCMO(_pd3dDevice, file, *_EF);

	m_count++;
}

MeshGO::~MeshGO()
{
	//model shouldn't need deleting as it's linked to by a unique_ptr
	m_count--;

	//okay I've just deleted the last CMOGO let's get rid of this
	if (m_count == 0 && s_pRasterState)
	{
		s_pRasterState->Release();
		s_pRasterState = nullptr;
	}

}

void MeshGO::Tick(GlobalData* _GD)
{
	GameObject::Tick(_GD);
}

void MeshGO::Draw(DrawData3D* _DD)
{
	//a dirty hack as the CMO model drawer breaks the depth stencil state
	ID3D11DepthStencilState *DSS = nullptr;
	UINT ref;
	//pick up a copy of the current state...
	_DD->m_pd3dImmediateContext->OMGetDepthStencilState(&DSS, &ref);

	m_model->Draw(_DD->m_pd3dImmediateContext, *(_DD->m_states), //graphics device and CommonStates reuqired by model system
		m_worldMat, //world transform to poisiton this model in the world
		_DD->m_cam->GetView(), _DD->m_cam->GetProj(), //veiw and projection matrix of the camera
		false, //NO! I don't want wireframe
		[&]() {_DD->m_pd3dImmediateContext->RSSetState(s_pRasterState);} //this VERY weird construction creates a function on the fly to set up the render states correctly else the model system overrides them BADLY
	);

	//...and put the depth stencil state back again
	_DD->m_pd3dImmediateContext->OMSetDepthStencilState(DSS, ref);
	if (DSS) //clear this copy away
	{
		DSS->Release();
	}
}