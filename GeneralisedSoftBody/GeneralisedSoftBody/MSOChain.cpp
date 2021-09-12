//=================================================================
// Mass/spring chain primitive
//=================================================================

#include "MSOChain.h"
#include "MSMesh.h"
#include "GlobalData.h"
#include "Triangle.h"
#include "Edge.h"
#include "Vertex.h"
#include "TriangulateVertices.h"

MSOChain::MSOChain(int _width, int _height, int _numSections, bool _generateInternals, GlobalData* _GD) : VertexMSO()
{
	m_name = "MSOChain";
	m_tag = ObjectTag::OT_NULL;
	m_layer = ObjectLayer::OL_NULL;

	SetScale(1.0f);

	m_width = _width;
	m_height = _height;

	m_numSections = _numSections;

	m_mesh = new MSMesh(this, _GD->m_MSOManager->m_fixIndex, _GD->m_MSOManager->m_fixformat, _GD->m_MSOManager->m_fixDimension);

	Initilaise(_generateInternals, _GD->m_IDevice);
}

MSOChain::~MSOChain()
{
}

//Generates all vertex and mass/spring structures
void MSOChain::Initilaise(bool _generateInternals, ID3D11Device* _ID)
{
	SetScale(1.0f);

	m_mesh->Set3D(false);
	TriangulateVertices* math = new TriangulateVertices();

	//Generates vertex mesh
	CreateVertices();
	CreateIntialVertices();
	UpdateNormals();
	math->CalculateTriangles(&m_triangles, m_vertices, m_indices, m_numPrims);

	//Generates mass/spring structure
	m_mesh->CreateMasses1D(m_numVerts, m_width, m_height, m_numSections);
	m_mesh->CreateSprings1D(m_numSections);

	delete(math);

	//Builds GPU Buffers
	BuildDIB(_ID, m_indices);
	BuildDVB(_ID, m_numVerts, m_vertices);

	//Setup Raster State
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	HRESULT hr = _ID->CreateRasterizerState(&rasterDesc, &m_pRasterState);
}

//Creates a strip of sections each defined by six vertices
void MSOChain::CreateVertices()
{
	//Calculate number of vertices and primatives
	m_numVerts = 6 * m_numSections;
	m_numPrims = m_numVerts / 3;
	m_vertices = new Vertex[m_numVerts];
	m_indices = new WORD[m_numVerts];

	//Using the standard VB shader set the tex-coords
	for (int i = 0; i < m_numVerts; i++)
	{
		m_indices[i] = i;
		m_vertices[i].texCoord = Vector2::One;
	}

	//Generates vertices in sections of six
	int vert = 0;
	float o = 0.0f;
	float p = 1.0f;

	Vector3 m_fudge = m_scale;

	for (int k = 0; k < m_numSections; k++)
	{
		m_vertices[vert].Color = Color(o, p, 1.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3(0.0f, (float)-(m_height * k), 0.0f);
		m_vertices[vert].Color = Color(o, p, 1.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3(0.0f, (float)-(m_height * (k + 1)), 0.0f);
		m_vertices[vert].Color = Color(o, p, 1.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3((float)m_width, (float)-(m_height * k), 0.0f);

		m_vertices[vert].Color = Color(o, p, 1.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3((float)m_width, (float)-(m_height * k), 0.0f);
		m_vertices[vert].Color = Color(o, p, 1.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3(0.0f, (float)-(m_height * (k + 1)), 0.0f);
		m_vertices[vert].Color = Color(o, p, 1.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3((float)m_width, (float)-(m_height * (k + 1)), 0.0f);

		float q = o;
		o = p;
		p = q;
	}

	return;
}