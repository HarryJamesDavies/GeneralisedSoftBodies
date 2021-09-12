//=================================================================
// Mass/spring cube primitive
//=================================================================

#include "MSOCube.h"
#include "MSMesh.h"
#include "GlobalData.h"
#include "Triangle.h"
#include "Edge.h"
#include "Vertex.h"
#include "TriangulateVertices.h"
#include "GlobalData.h"


MSOCube::MSOCube(int _width, int _height, int _depth, Vector3 _numSections, bool _generateInternals, GlobalData* _GD) : VertexMSO()
{
	m_name = "MSOCube";
	m_tag = ObjectTag::OT_NULL;
	m_layer = ObjectLayer::OL_NULL;

	SetScale(1.0f);

	m_width = _width;
	m_height = _height;
	m_depth = _depth;

	m_numSections = _numSections;

	m_mesh = new MSMesh(this, _GD->m_MSOManager->m_fixIndex, _GD->m_MSOManager->m_fixformat, _GD->m_MSOManager->m_fixDimension);

	Initilaise(_generateInternals, _GD->m_IDevice);
}

MSOCube::~MSOCube()
{
}

//Generates all vertex and mass/spring structures
void MSOCube::Initilaise(bool _generateInternals, ID3D11Device* _ID)
{
	m_mesh->Set3D(true);

	TriangulateVertices* math = new TriangulateVertices();

	//Generates vertex mesh
	CreateVertices();
	CreateIntialVertices();
	UpdateNormals();
	math->CalculateTriangles(&m_triangles, m_vertices, m_indices, m_numPrims);

	//Generates mass/spring structure
	m_mesh->CreateSurfaceMasses(m_numVerts, m_triangles.size(), m_triangles.begin(), m_triangles.end());
	m_mesh->CreateSurfaceSprings();
	m_mesh->UpdateNormals();
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

//Generates a mesh of vertices align to make a cube
void MSOCube::CreateVertices()
{
	//Calculate number of vertices and primatives
	m_numVerts = static_cast<int>((2 * 6 * (m_numSections.x) * (m_numSections.y)) + (2 * 6 * (m_numSections.x) * (m_numSections.z)) + (2 * 6 * (m_numSections.y) * (m_numSections.z)));
	m_numPrims = m_numVerts / 3;
	m_vertices = new Vertex[m_numVerts];
	m_indices = new WORD[m_numVerts];

	//Using the standard VB shader set the tex-coords
	for (int i = 0; i < m_numVerts; i++)
	{
		m_indices[i] = i;
		m_vertices[i].texCoord = Vector2::One;
	}

	//Generates cube two polygons per face each iteration
	int vert = 0;

	for (int y = 0; y < m_numSections.y; y++)
	{
		for (int x = 0; x < m_numSections.x; x++)
		{
			//front
			m_vertices[vert].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)x, m_height * (float)y, -m_depth *  m_numSections.z);
			m_vertices[vert].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)x, m_height * (float)(y + 1), -m_depth *  m_numSections.z);
			m_vertices[vert].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)(x + 1), m_height * (float)y, -m_depth *  m_numSections.z);

			m_vertices[vert].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)(x + 1), m_height * (float)y, -m_depth *  m_numSections.z);
			m_vertices[vert].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)x, m_height * (float)(y + 1), -m_depth *  m_numSections.z);
			m_vertices[vert].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)(x + 1), m_height * (float)(y + 1), -m_depth *  m_numSections.z);
		}
	}

	for (int y = 0; y < m_numSections.y; y++)
	{
		for (int x = 0; x < m_numSections.x; x++)
		{
			//back
			m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)(x + 1), m_height * (float)y, 0.0f);
			m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)(x + 1), m_height * (float)(y + 1), 0.0f);
			m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)x, m_height * (float)y, 0.0f);

			m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)x, m_height * (float)y, 0.0f);
			m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)(x + 1), m_height * (float)(y + 1), 0.0f);
			m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)x, m_height * (float)(y + 1), 0.0f);
		}
	}

	for (int z = 0; z < m_numSections.z; z++)
	{
		for (int x = 0; x < m_numSections.x; x++)
		{
			//top
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)(x + 1), m_height * m_numSections.y, -m_depth * (float)z);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)(x + 1), m_height * m_numSections.y, -m_depth * (float)(z + 1));
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)x, m_height * m_numSections.y, -m_depth * (float)z);

			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)x, m_height * m_numSections.y, -m_depth * (float)z);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)(x + 1), m_height * m_numSections.y, -m_depth * (float)(z + 1));
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)x, m_height * m_numSections.y, -m_depth * (float)(z + 1));
		}
	}

	for (int z = 0; z < m_numSections.z; z++)
	{
		for (int x = 0; x < m_numSections.x; x++)
		{
			//Bottom
			m_vertices[vert].Color = Color(1.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)x, 0.0f, -m_depth * (float)z);
			m_vertices[vert].Color = Color(1.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)x, 0.0f, -m_depth * (float)(z + 1));
			m_vertices[vert].Color = Color(1.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)(x + 1), 0.0f, -m_depth * (float)z);

			m_vertices[vert].Color = Color(1.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)(x + 1), 0.0f, -m_depth * (float)z);
			m_vertices[vert].Color = Color(1.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)x, 0.0f, -m_depth * (float)(z + 1));
			m_vertices[vert].Color = Color(1.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * (float)(x + 1), 0.0f, -m_depth * (float)(z + 1));
		}
	}

	for (int z = 0; z < m_numSections.z; z++)
	{
		for (int y = 0; y < m_numSections.y; y++)
		{
			//left
			m_vertices[vert].Color = Color(1.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, m_height * (float)y, -m_depth * (float)z);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, m_height * (float)(y + 1), -m_depth * (float)z);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, m_height * (float)y, -m_depth * (float)(z + 1));

			m_vertices[vert].Color = Color(1.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, m_height * (float)y, -m_depth * (float)(z + 1));
			m_vertices[vert].Color = Color(1.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, m_height * (float)(y + 1), -m_depth * (float)z);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.0f, m_height * (float)(y + 1), -m_depth * (float)(z + 1));
		}
	}

	for (int z = 0; z < m_numSections.z; z++)
	{
		for (int y = 0; y < m_numSections.y; y++)
		{
			//right
			m_vertices[vert].Color = Color(0.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * m_numSections.x, m_height * (float)y, -m_depth * (float)(z + 1));
			m_vertices[vert].Color = Color(0.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * m_numSections.x, m_height * (float)(y + 1), -m_depth * (float)(z + 1));
			m_vertices[vert].Color = Color(0.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * m_numSections.x, m_height * (float)y, -m_depth * (float)z);

			m_vertices[vert].Color = Color(0.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * m_numSections.x, m_height * (float)y, -m_depth * (float)z);
			m_vertices[vert].Color = Color(0.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * m_numSections.x, m_height * (float)(y + 1), -m_depth * (float)(z + 1));
			m_vertices[vert].Color = Color(0.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(m_width * m_numSections.x, m_height * (float)(y + 1), -m_depth * (float)z);
		}
	}

	return;
}