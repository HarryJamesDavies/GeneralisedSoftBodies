//=================================================================
//Base Game Object drawn using a Vertex and Index Buffer
//=================================================================

#include "VertexGO.h"
#include "d3dcompiler.h"
#include "DDSTextureLoader.h"
#include "GlobalData.h"

#include "Vertex.h"
#include "Camera.h"
#include "Light.h"
#include "ReadData.h"

#include <Effects.h>


#define DESTROY( x ) if( x ){ x->Release(); x = nullptr;}
#define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1)


//default vertexshader
ID3D11VertexShader*			VertexGO::s_pVertexShader = nullptr;
//default vertex layout
ID3D11InputLayout*			VertexGO::s_pVertexLayout = nullptr;
//default pixelshader
ID3D11PixelShader*			VertexGO::s_pPixelShader = nullptr;
//default texture (white square)
ID3D11ShaderResourceView*	VertexGO::s_pTextureRV = nullptr;
//deafult const buffer
//GPU side
ID3D11Buffer*				VertexGO::s_pConstantBuffer = nullptr;
//CPU side	
ConstantBuffer*				VertexGO::s_pCB = nullptr;
//default sampler
ID3D11SamplerState*			VertexGO::s_pSampler = nullptr;
//default raster state
ID3D11RasterizerState*		VertexGO::s_pRasterState = nullptr;

VertexGO::VertexGO()
{
	//set up Buffers
	m_VertexBuffer = NULL;
	m_IndexBuffer = NULL;
	m_numPrims = 0;

	m_pVertexShader = nullptr;
	m_pVertexLayout = nullptr;
	m_pPixelShader = nullptr;
	m_pTextureRV = nullptr;
	m_pConstantBuffer = nullptr;
	m_pCB = nullptr;
	m_pSampler = nullptr;
	m_pRasterState = nullptr;

	m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

VertexGO::~VertexGO()
{

	DESTROY(m_VertexBuffer);
	DESTROY(m_IndexBuffer);
	DESTROY(m_pVertexShader);
	DESTROY(m_pVertexLayout);
	DESTROY(m_pPixelShader);
	DESTROY(m_pTextureRV);
	DESTROY(m_pConstantBuffer);
	DESTROY(m_pSampler);
	DESTROY(m_pRasterState);
}

void VertexGO::Tick(GlobalData* _GD)
{
	GameObject::Tick(_GD);
	m_worldMat = m_fudge * m_worldMat;
}

void VertexGO::Draw(DrawData3D* _DD)
{
	//set raster state
	ID3D11RasterizerState* useRasterS = m_pRasterState ? m_pRasterState : s_pRasterState;
	_DD->m_pd3dImmediateContext->RSSetState(useRasterS);

	//set standard Constant Buffer to match this object
	s_pCB->world = m_worldMat.Transpose();
	s_pCB->rot = m_rotMat.Transpose();

	//Set vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	_DD->m_pd3dImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// Set index buffer
	_DD->m_pd3dImmediateContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	//update the Constant Buffer in classes that inherit from this then push to the GPU here
	ID3D11Buffer* useCB = m_pConstantBuffer ? m_pConstantBuffer : s_pConstantBuffer;
	void* useCBData = m_pCB ? m_pCB : s_pCB;

	_DD->m_pd3dImmediateContext->UpdateSubresource(useCB, 0, NULL, useCBData, 0, 0);
	_DD->m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, &useCB);
	_DD->m_pd3dImmediateContext->PSSetConstantBuffers(0, 1, &useCB);

	//unless it has it own set them to the static defaults

	//set primitive type used
	_DD->m_pd3dImmediateContext->IASetPrimitiveTopology(m_topology);

	//set  vertex layout
	//note that if you do use this you'll need to change the stride and offset above
	ID3D11InputLayout* useLayout = m_pVertexLayout ? m_pVertexLayout : s_pVertexLayout;
	_DD->m_pd3dImmediateContext->IASetInputLayout(useLayout);

	//set Vertex Shader
	ID3D11VertexShader* useVS = m_pVertexShader ? m_pVertexShader : s_pVertexShader;
	_DD->m_pd3dImmediateContext->VSSetShader(useVS, NULL, 0);

	//set Pixel Shader
	ID3D11PixelShader* usePS = m_pPixelShader ? m_pPixelShader : s_pPixelShader;
	_DD->m_pd3dImmediateContext->PSSetShader(usePS, NULL, 0);

	//set Texture
	ID3D11ShaderResourceView* useTex = m_pTextureRV ? m_pTextureRV : s_pTextureRV;
	_DD->m_pd3dImmediateContext->PSSetShaderResources(0, 1, &useTex);

	//set sampler
	ID3D11SamplerState* useSample = m_pSampler ? m_pSampler : s_pSampler;
	_DD->m_pd3dImmediateContext->PSSetSamplers(0, 1, &useSample);

	//and draw
	_DD->m_pd3dImmediateContext->DrawIndexed(3 * m_numPrims, 0, 0);//number here will need to change depending on the primative topology!
}

void VertexGO::Init(ID3D11Device* _ID)
{
	//set up all static stuff
	HRESULT hr;

	//default vertex shader
	auto pVertexShaderBuffer = DX::ReadData(L"BasicVertexShader.cso");
	_ID->CreateVertexShader(&pVertexShaderBuffer.front(), pVertexShaderBuffer.size(), NULL, &s_pVertexShader);

	//default pixelshader
	auto pPixelShaderBuffer = DX::ReadData(L"BasicPixelShader.cso");
	_ID->CreatePixelShader(&pPixelShaderBuffer.front(), pPixelShaderBuffer.size(), NULL, &s_pPixelShader);

	//default vertex layout
	_ID->CreateInputLayout(VertexLayout, ARRAYSIZE(VertexLayout), &pVertexShaderBuffer.front(),
		pVertexShaderBuffer.size(), &s_pVertexLayout);

	//default texture (white square)
#ifdef DEBUG
	hr = CreateDDSTextureFromFile(_ID, L"../Debug/white.dds", nullptr, &s_pTextureRV);
#else
	hr = CreateDDSTextureFromFile(_ID, L"../Release/white.dds", nullptr, &s_pTextureRV);
#endif

	//deafult const buffer
	//GPU side
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = _ID->CreateBuffer(&bd, NULL, &s_pConstantBuffer);
	//CPU side
	s_pCB = new ConstantBuffer();
	ZeroMemory(s_pCB, sizeof(ConstantBuffer));

	//default sampler
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(SamDesc));
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.MipLODBias = 0.0f;
	SamDesc.MaxAnisotropy = 1;
	SamDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SamDesc.BorderColor[0] = SamDesc.BorderColor[1] = SamDesc.BorderColor[2] = SamDesc.BorderColor[3] = 0;
	SamDesc.MinLOD = 0;
	SamDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = _ID->CreateSamplerState(&SamDesc, &s_pSampler);

	//Setup Raster State
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	hr = _ID->CreateRasterizerState(&rasterDesc, &s_pRasterState);
}

void VertexGO::UpdateConstantBuffer(DrawData3D* _DD)
{
	//you'll need your own version of this if you use a different Constant Buffer
	s_pCB->view = _DD->m_cam->GetView().Transpose();
	s_pCB->projection = _DD->m_cam->GetProj().Transpose();
	if (_DD->m_light)
	{
		s_pCB->lightCol = _DD->m_light->GetColour();
		s_pCB->lightPos = _DD->m_light->GetPos();
		s_pCB->ambientCol = _DD->m_light->GetAmbCol();
	}
}

void VertexGO::CleanUp()
{
	//clear away all the static objects
	DESTROY(s_pVertexShader);
	DESTROY(s_pVertexLayout);
	DESTROY(s_pPixelShader);
	DESTROY(s_pTextureRV);
	DESTROY(s_pConstantBuffer);
	if (s_pCB)
	{
		delete s_pCB;
		s_pCB = nullptr;
	}
	DESTROY(s_pSampler);
	DESTROY(s_pRasterState);
}

void VertexGO::BuildIB(ID3D11Device* _ID, void* _indices)
{
	//structures from creating buffers
	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA InitData;
	HRESULT hr = S_OK;

	//build index buffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 3 * m_numPrims;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = _indices;
	hr = _ID->CreateBuffer(&bd, &InitData, &m_IndexBuffer);
}

void VertexGO::BuildDIB(ID3D11Device* _ID, void* _indices)
{
	//structures from creating buffers
	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA InitData;
	HRESULT hr = S_OK;

	//build index buffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(WORD) * 3 * m_numPrims;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	InitData.pSysMem = _indices;
	hr = _ID->CreateBuffer(&bd, &InitData, &m_IndexBuffer);
}

void VertexGO::BuildVB(ID3D11Device* _ID, int _numVerts, void* _vertices)
{
	//structures from creating buffers
	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA InitData;
	HRESULT hr = S_OK;

	//build vertex buffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * _numVerts;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = _vertices;
	hr = _ID->CreateBuffer(&bd, &InitData, &m_VertexBuffer);
}

void VertexGO::BuildDVB(ID3D11Device* _ID, int _numVerts, void* _vertices)
{
	//structures from creating buffers
	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA InitData;
	HRESULT hr = S_OK;

	//build vertex buffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * _numVerts;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = _vertices;
	hr = _ID->CreateBuffer(&bd, &InitData, &m_VertexBuffer);
}