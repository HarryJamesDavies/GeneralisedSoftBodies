//=================================================================
// Mass/spring square primitive
//=================================================================

#include "MSOSquare.h"
#include "MSMesh.h"
#include "GlobalData.h"
#include "Triangle.h"
#include "Edge.h"
#include "Vertex.h"
#include "TriangulateVertices.h"

MSOSquare::MSOSquare(int _width, int _height, Vector2 _numSections, bool _generateInternals, GlobalData* _GD) : VertexMSO()
{
	m_name = "MSOSquare";
	m_tag = ObjectTag::OT_NULL;
	m_layer = ObjectLayer::OL_NULL;

	SetScale(1.0f);

	m_width = _width;
	m_height = _height;

	m_numSections = _numSections;

	m_mesh = new MSMesh(this, _GD->m_MSOManager->m_fixIndex, _GD->m_MSOManager->m_fixformat, _GD->m_MSOManager->m_fixDimension);

	Initilaise(_generateInternals, _GD->m_IDevice);
}

MSOSquare::~MSOSquare()
{
}

//Generates all vertex and mass/spring structures
void MSOSquare::Initilaise(bool _generateInternals, ID3D11Device* _ID)
{
	m_mesh->Set3D(false);

	TriangulateVertices* math = new TriangulateVertices();

	//Generates vertex mesh
	CreateVertices();
	CreateIntialVertices();
	UpdateNormals();
	math->CalculateTriangles(&m_triangles, m_vertices, m_indices, m_numPrims);

	//Generates mass/spring structure
	m_mesh->CreateSurfaceMasses(m_numVerts, m_triangles.size(), m_triangles.begin(), m_triangles.end());
	m_mesh->CreateSurfaceSprings();
	if (_generateInternals)
	{
		m_mesh->CreateInternalMasses();
		m_mesh->CreateInternalSprings();
	}

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

//Generates a mesh of vertices to assemble a Square
void MSOSquare::CreateVertices()
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
	float o = 0.0f;
	float p = 1.0f;

	for (int y = 0; y < m_numSections.y; y++)
	{
		for (int x = 0; x < m_numSections.x; x++)
		{
			m_vertices[vert].Color = Color(o, p, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(m_width * x), (float)-(m_height * y), 0.0f);
			m_vertices[vert].Color = Color(o, p, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(m_width * x), (float)-(m_height * (y + 1)), 0.0f);
			m_vertices[vert].Color = Color(o, p, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(m_width * (x + 1)), (float)-(m_height * y), 0.0f);

			m_vertices[vert].Color = Color(o, p, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(m_width * (x + 1)), (float)-(m_height * y), 0.0f);
			m_vertices[vert].Color = Color(o, p, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(m_width * x), (float)-(m_height * (y + 1)), 0.0f);
			m_vertices[vert].Color = Color(o, p, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(m_width * (x + 1)), (float)-(m_height * (y + 1)), 0.0f);

			float q = o;
			o = p;
			p = q;
		}
	}

	return;
}