//=================================================================
// Mass/spring MSO Text file object
//=================================================================

#include "MSOFile.h"
#include "MSMesh.h"
#include "GlobalData.h"
#include "Triangle.h"
#include "Edge.h"
#include "Vertex.h"
#include "TriangulateVertices.h"
#include <fstream>
#include <sstream>
#include <string>


MSOFile::MSOFile(const char* _fileName, std::string _name, GlobalData* _GD) : VertexMSO()
{
	m_fileName = _fileName;

	m_name = _name;
	m_tag = ObjectTag::OT_NULL;
	m_layer = ObjectLayer::OL_NULL;

	SetScale(1.0f);

	m_mesh = new MSMesh(this, _GD->m_MSOManager->m_fixIndex, _GD->m_MSOManager->m_fixformat, _GD->m_MSOManager->m_fixDimension);

	Initilaise(_GD->m_IDevice);
}

MSOFile::~MSOFile()
{

}

//Generates all vertex and mass/spring structures
void MSOFile::Initilaise(ID3D11Device* _ID)
{
	TriangulateVertices* math = new TriangulateVertices();

	//Generates vertex mesh
	CreateVertices();
	CreateIntialVertices();
	math->CalculateTriangles(&m_triangles, m_vertices, m_indices, m_numPrims);
	UpdateNormals();

	//Generates mass/spring structure
	m_mesh->CreateFromFile(m_fileName);
	m_mesh->UpdateNormals();

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

//Reads in vertices and indices from MSO Text file
void MSOFile::CreateVertices()
{
	std::ifstream infile;
	infile.open(m_fileName);

	while (infile.good())
	{
		//Loops until then of the end of the file is reached
		std::string line;
		while (std::getline(infile, line))
		{
			std::istringstream in(line);

			std::string type;
			in >> type;

			if (type == "3D:")
			{
				//Reads in flag which determines whether the object has internal space
				int temp3D = 0;
				in >> temp3D;
				m_mesh->Set3D(static_cast<bool>(temp3D));
			}
			else if (type == "VERTICES:")
			{
				//Reads in and create vertex data
				in >> m_numVerts;
				m_vertices = new Vertex[m_numVerts];
				for (int vertexIter = 0; vertexIter <= m_numVerts - 1; vertexIter++)
				{
					std::getline(infile, line);

					std::getline(infile, line);
					std::istringstream vertex(line);		
					vertex.ignore(line.length(), ':');
					vertex >> m_vertices[vertexIter].Pos.x >> m_vertices[vertexIter].Pos.y >> m_vertices[vertexIter].Pos.z;

					std::getline(infile, line);
					std::istringstream colour(line);
					colour.ignore(line.length(), ':');
					colour >> m_vertices[vertexIter].Color.x >> m_vertices[vertexIter].Color.y >> m_vertices[vertexIter].Color.z >> m_vertices[vertexIter].Color.w;
					m_vertices[vertexIter].texCoord = Vector2::One;
				}
			}
			else if (type == "INDICES:")
			{
				//Reads in and creates indicies data
				int numIndicies = 0;
				in >> numIndicies;
				m_numPrims = numIndicies / 3;
				m_indices = new WORD[numIndicies];
				for (int indexIter = 0; indexIter <= numIndicies - 1; indexIter++)
				{
					std::getline(infile, line);
					std::istringstream single(line);
					single.ignore(line.length(), ':');
					single >> m_indices[indexIter];
				}
			}
		}
	}

	infile.close();
}