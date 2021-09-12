//=================================================================
// Vertex based plane object
//=================================================================

#include "VertexPlane.h"
#include "GlobalData.h"
#include "PhysicsData.h"
#include "ObjectLayer.h"

VertexPlane::VertexPlane(int _width, int _height, Vector2 _numSections, ID3D11Device* _ID, GlobalData* _GD) : VertexGO()
{
	m_name = "Plane";
	m_tag = ObjectTag::OT_NULL;
	m_layer = ObjectLayer::OL_NULL;

	SetScale(1.0f);

	m_width = _width;
	m_height = _height;
	m_numSections = _numSections;

	init(_ID);
}

VertexPlane::~VertexPlane()
{
}

void VertexPlane::init(ID3D11Device* _ID)
{
	CreateVertices();

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

void VertexPlane::CreateVertices()
{
	//Calculate number of vertices and primatives
	m_numVerts = static_cast<int>(6 * (m_numSections.x) * (m_numSections.y));
	m_numPrims = m_numVerts / 3;
	m_vertices = new Vertex[m_numVerts];
	m_indices = new WORD[m_numVerts];

	//Using the standard VB shader set the tex-coords
	for (int i = 0; i < m_numVerts; i++)
	{
		m_indices[i] = i;
		m_vertices[i].texCoord = Vector2::One;
	}

	//Generates columns then rows of square two polygons at a time
	int vert = 0;

	for (int y = 0; y < m_numSections.y; y++)
	{
		for (int x = 0; x < m_numSections.x; x++)
		{
			m_vertices[vert].Color = Color(0.5f, 0.5f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(m_width * x), 0.0f, (float)-(m_height * y));
			m_vertices[vert].Color = Color(0.5f, 0.5f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(m_width * x), 0.0f, (float)-(m_height * (y + 1)));
			m_vertices[vert].Color = Color(0.5f, 0.5f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(m_width * (x + 1)), 0.0f, (float)-(m_height * y));

			m_vertices[vert].Color = Color(0.5f, 0.5f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(m_width * (x + 1)), 0.0f, (float)-(m_height * y));
			m_vertices[vert].Color = Color(0.5f, 0.5f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(m_width * x), 0.0f, (float)-(m_height * (y + 1)));
			m_vertices[vert].Color = Color(0.5f, 0.5f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(m_width * (x + 1)), 0.0f, (float)-(m_height * (y + 1)));
		}
	}

	return;
}

void VertexPlane::CalculateNormals()
{
	//Calculate the normals for the basic lighting in the base shader
	for (UINT i = 0; i < m_numPrims; i++)
	{
		WORD V1 = 3 * i;
		WORD V2 = 3 * i + 1;
		WORD V3 = 3 * i + 2;

		//Build normals
		Vector3 norm;
		Vector3 vec1 = m_vertices[V1].Pos - m_vertices[V2].Pos;
		Vector3 vec2 = m_vertices[V3].Pos - m_vertices[V2].Pos;
		norm = vec1.Cross(vec2);
		norm.Normalize();

		m_vertices[V1].Norm = norm;
		m_vertices[V2].Norm = norm;
		m_vertices[V3].Norm = norm;
	}
}

void VertexPlane::Tick(GlobalData* _GD)
{ 
	CalculateNormals(); 

	VertexGO::Tick(_GD);
}