//=================================================================
//Base Game Object drawn using a Vertex and Index Buffer
//=================================================================

#ifndef _VERTEX_GO_H_
#define _VERTEX_GO_H_
#include "GameObject.h"
#include "DrawData3D.h"
#include "ConstantBuffer.h"
#include "Vertex.h"

using namespace DirectX;

struct GlobalData;

class VertexGO :public GameObject
{
public:
	VertexGO();
	virtual ~VertexGO();

	virtual void Tick(GlobalData* _GD);
	virtual void Draw(DrawData3D* _DD);

	//Set up and then destroy all static default render stuff for VBGOs
	static void Init(ID3D11Device* _GD);//Graphics Device NOT GameData
	static void CleanUp();

	//Update static const buffer required for deafult rendering
	static void UpdateConstantBuffer(DrawData3D* _DD);

	Vertex* GetVertex(int i) { return &m_vertices[i]; }
	Vector3 GetVertexPos(int i) { return m_vertices[i].Pos; };
	void SetVertexPos(int i, Vector3 _pos) { m_vertices[i].Pos = _pos; };
	int GetVertexCount() { return m_numVerts; };

	int GetIndexCount() { return m_numPrims * 3; };
	int GetIndex(int i) { return m_indices[i]; };

	Vector3 GetVertexNorm(int i) { return  m_vertices[i].Norm; };

protected:

	//Direct X information for the model for this GameObject
	ID3D11Buffer* m_VertexBuffer;
	ID3D11Buffer* m_IndexBuffer;
	UINT m_numPrims;

	//Vertex topology in VB
	D3D_PRIMITIVE_TOPOLOGY m_topology;

	//default vertexshader
	static ID3D11VertexShader*     s_pVertexShader;
	//default vertex layout
	static ID3D11InputLayout*      s_pVertexLayout;
	//default pixelshader
	static ID3D11PixelShader*      s_pPixelShader;
	//default texture (white square)
	static ID3D11ShaderResourceView* s_pTextureRV;
	//deafult const buffer
	static ID3D11Buffer*           s_pConstantBuffer;	//GPU side
	static ConstantBuffer*			s_pCB;				//CPU side
														//default sampler
	static ID3D11SamplerState*		s_pSampler;
	//default raster state
	static ID3D11RasterizerState*  s_pRasterState;

	//my vertexshader
	ID3D11VertexShader*     m_pVertexShader;
	//my vertex layout
	ID3D11InputLayout*      m_pVertexLayout;
	//my pixelshader
	ID3D11PixelShader*      m_pPixelShader;
	//my texture
	ID3D11ShaderResourceView* m_pTextureRV;
	//my const buffer
	ID3D11Buffer*           m_pConstantBuffer; //GPU side
	void*					m_pCB;//CPU side
								  //my sampler
	ID3D11SamplerState*		m_pSampler;
	//my raster state
	ID3D11RasterizerState*  m_pRasterState;

	//once populated build an Index Buffer
	void BuildIB(ID3D11Device* _GD, void* _indices);

	//once populated build an Index Buffer
	void BuildDIB(ID3D11Device* _GD, void* _indices);

	//once populated build a Vertex Buffer
	void BuildVB(ID3D11Device* _GD, int _numVerts, void* _vertices);

	//once populated build a Vertex Buffer
	void BuildDVB(ID3D11Device* _GD, int _numVerts, void* _vertices);

	WORD* m_indices;
	Vertex* m_vertices;
	int m_maxVert;
	int m_numVerts;
};

#endif