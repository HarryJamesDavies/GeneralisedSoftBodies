//=================================================================
// Managers the user interface and interactions
//=================================================================

#include "GUIManager.h"
#include "MSOManager.h"
#include "MSOTypes.h"
#include "GlobalData.h"
#include "FixDimension.h"
#include "FixFormat.h"

#include <fstream>
#include <sstream>
#include <string>

GUIManager::GUIManager()
{
	m_createPrimitive = true;
	m_spawnPrimitive = false;

	m_createFile = false;
	m_spawnFile = false;

	//Resets file name variable
	m_fileName = new char[100];
	for (int iter = 0; iter <= 102; iter++)
	{
		m_fileName[iter] = NULL;
	}
}

GUIManager::~GUIManager()
{
	m_GD = nullptr;
	TwTerminate();
}

//Initialise GUI Systems
void GUIManager::Initialise(GlobalData* _GD)
{
	m_GD = _GD;

	//Initialise AntTweakBar
	TwInit(TW_DIRECT3D11, m_GD->m_IDevice);
	TwWindowSize(m_GD->m_width, m_GD->m_height);

	CreateMSOManagerBar();
	CreatePhysicsBar();
}

//Checks for changes in flags and enums
void GUIManager::Update()
{
	//Determines whether a new Primitive tool bar should be spawned
	if (m_createPrimitive)
	{
		m_createPrimitive = false;

		//Deletes previous object tool bar
		TwDeleteBar(msoBar);
		msoBar = nullptr;

		//Creates primitve tool bar
		switch (m_GD->m_MSOManager->m_currentType)
		{
		case MSOTypes::CHAIN:
		{
			CreateChainBar();
			break;
		}
		case MSOTypes::SQUARE:
		{
			CreateSquareBar();
			break;
		}
		case MSOTypes::CUBE:
		{
			CreateCubeBar();
			break;
		}
		default:
		{
			break;
		}
		}
	}

	//Spawns primitive MSO
	if (m_spawnPrimitive)
	{
		m_spawnPrimitive = false;

		//Pause object when spawned into the scene
		if (m_pauseOnSpawn)
		{
			m_GD->m_MSOManager->m_pauseObject = true;
		}

		m_GD->m_MSOManager->CreateMSOPrimitive(m_GD);
	}

	//Creates MSO file too bar
	if (m_createFile)
	{
		m_createFile = false;

		//Deletes previous object tool bar
		TwDeleteBar(msoBar);
		msoBar = nullptr;

		//Resets file name
		m_fileName = new char[100];
		for (int iter = 0; iter <= 102; iter++)
		{
			m_fileName[iter] = NULL;
		}

		CreateFileBar();
	}

	//Spawns MSO file object
	if (m_spawnFile)
	{
		m_spawnFile = false;

		//Pause object when spawned into the scene
		if (m_pauseOnSpawn)
		{
			m_GD->m_MSOManager->m_pauseObject = true;
		}

		//Resets file name
		char* fileName = new char[100];
		for (int iter = 0; iter <= 100; iter++)
		{
			fileName[iter] = NULL;
		}

		char temp[19] = "../Assets/MSO/.txt";

		//Dynamically reates prefix and suffix for file name
		int presetIter = 0;
		int dynamicIter = 0;
		for (int iter = 0; iter <= 100; iter++)
		{
			if (iter < 14)
			{
				fileName[iter] = temp[presetIter];
				presetIter++;
			}
			else
			{
				if (m_fileName[dynamicIter] != NULL)
				{
					fileName[iter] = m_fileName[dynamicIter];
					dynamicIter++;
				}
				else
				{
					if (presetIter < 19)
					{
						fileName[iter] = temp[presetIter];
						presetIter++;
					}
				}
			}
		}

		//Checks whether file exists
		std::ifstream infile;
		infile.open(fileName);

		if (infile.good())
		{
			infile.close();
			m_GD->m_MSOManager->CreateMSOFile(fileName, m_fileName, m_GD);
		}
	}


	//Creates FBX tool bar
	if (m_createFBX)
	{
		m_createFBX = false;

		//Deletes previous object tool bar
		TwDeleteBar(msoBar);
		msoBar = nullptr;

		//Resets file name
		m_fileName = new char[100];
		for (int iter = 0; iter <= 102; iter++)
		{
			m_fileName[iter] = NULL;
		}

		CreateFBXBar();
	}

	//Spawns FBX MSO 
	if (m_spawnFBX)
	{
		m_spawnFBX = false;

		//Pause object when spawned into the scene
		if (m_pauseOnSpawn)
		{
			m_GD->m_MSOManager->m_pauseObject = true;
		}

		//Resets file name
		char* fileName = new char[100];
		for (int iter = 0; iter <= 100; iter++)
		{
			fileName[iter] = NULL;
		}

		char temp[21] = "../Assets/ASCII/.ase";

		//Dynamically reates prefix and suffix for file name
		int presetIter = 0;
		int dynamicIter = 0;
		for (int iter = 0; iter <= 100; iter++)
		{
			if (iter < 16)
			{
				fileName[iter] = temp[presetIter];
				presetIter++;
			}
			else
			{
				if (m_fileName[dynamicIter] != NULL)
				{
					fileName[iter] = m_fileName[dynamicIter];
					dynamicIter++;
				}
				else
				{
					if (presetIter < 21)
					{
						fileName[iter] = temp[presetIter];
						presetIter++;
					}
				}
			}
		}

		//Checks whether file exists
		std::ifstream infile;
		infile.open(fileName);

		if (infile.good())
		{
			infile.close();
			m_GD->m_MSOManager->CreateMSOFBX(fileName, m_fileName, m_GD);
		}
	}

	//Save MSO to Text file
	if (m_saveMSO)
	{
		m_saveMSO = false;
		m_GD->m_MSOManager->SaveMSO(m_GD);
	}
}

//Creates MSO Manager tool bar
void GUIManager::CreateMSOManagerBar()
{
	msoManagerBar = TwNewBar("MSO Parameters");
	int position[2] = { 20, 20 };
	TwSetParam(msoManagerBar, NULL, "position", TW_PARAM_INT32, 2, &position);

	TwAddVarRW(msoManagerBar, "Pause", TW_TYPE_BOOL16, &m_GD->m_pause, "group=SystemParameters key=InternalFlag");

	TwAddVarRW(msoManagerBar, "Spawn Internal Structure", TW_TYPE_BOOL16, &m_GD->m_MSOManager->m_internal, "group=ObjectParameters key=InternalFlag");
	TwAddVarRW(msoManagerBar, "Pause Object", TW_TYPE_BOOL16, &m_GD->m_MSOManager->m_pauseObject, "group=ObjectParameters key=InternalFlag");
	TwAddVarRW(msoManagerBar, "Pause On Spawn", TW_TYPE_BOOL16, &m_pauseOnSpawn, "group=ObjectParameters key=InternalFlag");
	TwAddVarRW(msoManagerBar, "Reset", TW_TYPE_BOOL16, &m_GD->m_MSOManager->m_resetVertices, "group=ObjectParameters key=InternalFlag");

	TwAddVarRW(msoManagerBar, "Create File Bar", TW_TYPE_BOOL16, &m_createFile, "group=SpawnFileObject key=InternalFlag");

	TwAddVarRW(msoManagerBar, "Create FBX Bar", TW_TYPE_BOOL16, &m_createFBX, "group=SpawnFBXObject key=InternalFlag");

	TwEnumVal typesEV[] = { { MSOTypes::CHAIN, "Chain" },{ MSOTypes::SQUARE, "Square" },{ MSOTypes::CUBE, "Cube" } };
	TwType typesType;
	typesType = TwDefineEnum("ObjectType", typesEV, MSOTypes::Count);
	TwAddVarRW(msoManagerBar, "Object Type", typesType, &m_GD->m_MSOManager->m_currentType, "group=SpawnPrimitiveObject key=ObjectType");
	TwAddVarRW(msoManagerBar, "Create Primitive", TW_TYPE_BOOL16, &m_createPrimitive, "group=SpawnPrimitiveObject key=InternalFlag");
}

//Creates physics tool bar
void GUIManager::CreatePhysicsBar()
{
	physicsBar = TwNewBar("Physics Parameters");
	int position[2] = { 20, static_cast<int>(m_GD->m_height) - 600 };
	TwSetParam(physicsBar, NULL, "position", TW_PARAM_INT32, 2, &position);

	//Inititalise Force Parameters
	TwAddVarRW(physicsBar, "Drag Coefficient", TW_TYPE_FLOAT, &m_GD->m_physicsManager->m_physicsData.m_dragCoefficient, "min=0.0  max=5.0  step=0.1 group=GeneralParameters key=DragCoefficient");
	TwAddVarRW(physicsBar, "Velocity Limit", TW_TYPE_FLOAT, &m_GD->m_physicsManager->m_physicsData.m_velocityLimit, "min=5.0  max=1000.0  step=10.0 group=GeneralParameters key=VelocityLimit");
	TwAddVarRW(physicsBar, "Gravity Coefficient", TW_TYPE_FLOAT, &m_GD->m_physicsManager->m_physicsData.m_gravityCoefficient, "min=-20.0  max=20.0  step=1.0 group=GeneralParameters key=GraviyCoefficient");

	TwAddVarRW(physicsBar, "Spring Coefficient", TW_TYPE_FLOAT, &m_GD->m_physicsManager->m_physicsData.m_springCoefficient, "min=0.0  max=1000.0  step=1.0 group=SpringParameters key=SpringCoefficient");
	TwAddVarRW(physicsBar, "Pressure Coefficient", TW_TYPE_FLOAT, &m_GD->m_physicsManager->m_physicsData.m_pressureCoefficient, "min=0.0  max=1000.0  step=1.0 group=SpringParameters key=PressureCoefficient");
	TwAddVarRW(physicsBar, "Damping Coefficient", TW_TYPE_FLOAT, &m_GD->m_physicsManager->m_physicsData.m_dampingCoefficient, "min=0.0  max=100.0  step=1.0 group=SpringParameters key=DampingCoefficient");

	TwAddVarRW(physicsBar, "Force Field Coefficient", TW_TYPE_FLOAT, &m_GD->m_physicsManager->m_physicsData.m_forceFieldCoefficient, "min=0.0  max=100.0  step=1.0 group=ForceFieldParameters key=ForceFieldCoefficient");
	TwAddVarRW(physicsBar, "Force Field Radius", TW_TYPE_FLOAT, &m_GD->m_physicsManager->m_physicsData.m_forceFieldRadius, "min=0.0  max=100.0  step=1.0 group=ForceFieldParameters key=ForceFieldCoefficient");

	//Inititalise Wind Parameters
	TwAddVarRW(physicsBar, "Wind Force Min", TW_TYPE_FLOAT, &m_GD->m_physicsManager->m_physicsData.m_windCoefficientMin, "min=-100.0  max=100.0  step=0.1 group=WindParameters key=Wind Force");
	TwAddVarRW(physicsBar, "Wind Force Max", TW_TYPE_FLOAT, &m_GD->m_physicsManager->m_physicsData.m_windCoefficientMax, "min=-100.0  max=100.0  step=0.1 group=WindParameters key=Wind Force");
	TwAddVarRW(physicsBar, "Wind Direction", TW_TYPE_DIR3F, &m_GD->m_physicsManager->m_physicsData.m_windDirection, "opened=false group=WindParameters key=Wind Direction");
}

//Creates MSO file object tool bar
void GUIManager::CreateFileBar()
{
	msoBar = TwNewBar("MSO File Parameters");
	int position[2] = { 240, 20 };
	TwSetParam(msoBar, NULL, "position", TW_PARAM_INT32, 2, &position);

	char temp[100];

	TwAddVarRW(msoBar, "Spawn File Object", TW_TYPE_BOOL16, &m_spawnFile, "group=ObjectParameters key=InternalFlag");
	TwAddVarRW(msoBar, "File Name", TW_TYPE_CSSTRING(sizeof(temp)), m_fileName, "group=ObjectParameters key=InternalFlag");

	TwEnumVal formatEV[] = { { FixFormat::None, "None" },{ FixFormat::Single, "Single" },{ FixFormat::Level, "Level" },{ FixFormat::Above, "Above" },{ FixFormat::Below, "Below" },{ FixFormat::All, "All" } };
	TwType formatType = TwDefineEnum("Fix Format", formatEV, 6);
	TwAddVarRW(msoBar, "Fix Format", formatType, &m_GD->m_MSOManager->m_fixformat, "group=FixParameters key=ObjectType");
	TwAddVarRW(msoBar, "Fix Index", TW_TYPE_INT32, &m_GD->m_MSOManager->m_fixIndex, "min=0.0  max=10000.0  step=1.0 group=FixParameters key=SectionsZ");
	TwEnumVal dimensionEV[] = { { FixDimension::X, "X" },{ FixDimension::Y, "Y" },{ FixDimension::Z, "Z" } };
	TwType dimensionType = TwDefineEnum("Fix Dimension", dimensionEV, 3);
	TwAddVarRW(msoBar, "Fix Dimension", dimensionType, &m_GD->m_MSOManager->m_fixDimension, "group=FixParameters key=ObjectType");

	TwAddVarRW(msoBar, "Save Object", TW_TYPE_BOOL16, &m_saveMSO, "group=Save key=InternalFlag");
}

//Creates MSO FBX object tool bar
void GUIManager::CreateFBXBar()
{
	msoBar = TwNewBar("MSO FBX Parameters");
	int position[2] = { 240, 20 };
	TwSetParam(msoBar, NULL, "position", TW_PARAM_INT32, 2, &position);

	char temp[100];

	TwAddVarRW(msoBar, "Spawn File Object", TW_TYPE_BOOL16, &m_spawnFBX, "group=ObjectParameters key=InternalFlag");
	TwAddVarRW(msoBar, "File Name", TW_TYPE_CSSTRING(sizeof(temp)), m_fileName, "group=ObjectParameters key=InternalFlag");

	TwEnumVal formatEV[] = { { FixFormat::None, "None" },{ FixFormat::Single, "Single" },{ FixFormat::Level, "Level" },{ FixFormat::Above, "Above" },{ FixFormat::Below, "Below" },{ FixFormat::All, "All" } };
	TwType formatType = TwDefineEnum("Fix Format", formatEV, 6);
	TwAddVarRW(msoBar, "Fix Format", formatType, &m_GD->m_MSOManager->m_fixformat, "group=FixParameters key=ObjectType");
	TwAddVarRW(msoBar, "Fix Index", TW_TYPE_INT32, &m_GD->m_MSOManager->m_fixIndex, "min=0.0  max=10000.0  step=1.0 group=FixParameters key=SectionsZ");
	TwEnumVal dimensionEV[] = { { FixDimension::X, "X" },{ FixDimension::Y, "Y" },{ FixDimension::Z, "Z" } };
	TwType dimensionType = TwDefineEnum("Fix Dimension", dimensionEV, 3);
	TwAddVarRW(msoBar, "Fix Dimension", dimensionType, &m_GD->m_MSOManager->m_fixDimension, "group=FixParameters key=ObjectType");

	TwAddVarRW(msoBar, "Save Object", TW_TYPE_BOOL16, &m_saveMSO, "group=Save key=InternalFlag");
}

//Creates MSO chain primitive tool bar
void GUIManager::CreateChainBar()
{
	msoBar = TwNewBar("MSO Chain Parameters");
	int position[2] = { 240, 20 };
	TwSetParam(msoBar, NULL, "position", TW_PARAM_INT32, 2, &position);

	TwAddVarRW(msoBar, "Spawn Primitive", TW_TYPE_BOOL16, &m_spawnPrimitive, "group=ObjectParameters key=InternalFlag");
	TwAddVarRW(msoBar, "Section Width", TW_TYPE_INT8, &m_GD->m_MSOManager->m_width, "min=1.0  max=100.0  step=1.0 group=ObjectParameters key=SectionWidth");
	TwAddVarRW(msoBar, "Section Height", TW_TYPE_INT8, &m_GD->m_MSOManager->m_height, "min=1.0  max=100.0  step=1.0 group=ObjectParameters key=SectionHeight");
	TwAddVarRW(msoBar, "Sections X", TW_TYPE_INT8, &m_GD->m_MSOManager->m_sectionsX, "min=1.0  max=20.0  step=1.0 group=ObjectParameters key=SectionsX");

	TwEnumVal formatEV[] = { { FixFormat::None, "None" },{ FixFormat::Single, "Single" },{ FixFormat::Level, "Level" },{ FixFormat::Above, "Above" },{ FixFormat::Below, "Below" },{ FixFormat::All, "All" } };
	TwType formatType = TwDefineEnum("Fix Format", formatEV, 6);
	TwAddVarRW(msoBar, "Fix Format", formatType, &m_GD->m_MSOManager->m_fixformat, "group=FixParameters key=ObjectType");
	TwAddVarRW(msoBar, "Fix Index", TW_TYPE_INT32, &m_GD->m_MSOManager->m_fixIndex, "min=0.0  max=10000.0  step=1.0 group=FixParameters key=SectionsZ");
	TwEnumVal dimensionEV[] = { { FixDimension::X, "X" },{ FixDimension::Y, "Y" },{ FixDimension::Z, "Z" } };
	TwType dimensionType = TwDefineEnum("Fix Dimension", dimensionEV, 3);
	TwAddVarRW(msoBar, "Fix Dimension", dimensionType, &m_GD->m_MSOManager->m_fixDimension, "group=FixParameters key=ObjectType");

	TwAddVarRW(msoBar, "Save Object", TW_TYPE_BOOL16, &m_saveMSO, "group=Save key=InternalFlag");
}

//Creates MSO square primitive tool bar
void GUIManager::CreateSquareBar()
{
	msoBar = TwNewBar("MSO Square Parameters");
	int position[2] = { 240, 20 };
	TwSetParam(msoBar, NULL, "position", TW_PARAM_INT32, 2, &position);

	TwAddVarRW(msoBar, "Spawn Primitive", TW_TYPE_BOOL16, &m_spawnPrimitive, "group=ObjectParameters key=InternalFlag");
	TwAddVarRW(msoBar, "Section Width", TW_TYPE_INT8, &m_GD->m_MSOManager->m_width, "min=1.0  max=100.0  step=1.0 group=ObjectParameters key=SectionWidth");
	TwAddVarRW(msoBar, "Section Height", TW_TYPE_INT8, &m_GD->m_MSOManager->m_height, "min=1.0  max=100.0  step=1.0 group=ObjectParameters key=SectionHeight");
	TwAddVarRW(msoBar, "Sections X", TW_TYPE_INT8, &m_GD->m_MSOManager->m_sectionsX, "min=1.0  max=20.0  step=1.0 group=ObjectParameters key=SectionsX");
	TwAddVarRW(msoBar, "Sections Y", TW_TYPE_INT8, &m_GD->m_MSOManager->m_sectionsY, "min=1.0  max=20.0  step=1.0 group=ObjectParameters key=SectionsY");

	TwEnumVal formatEV[] = { { FixFormat::None, "None" },{ FixFormat::Single, "Single" },{ FixFormat::Level, "Level" },{ FixFormat::Above, "Above" },{ FixFormat::Below, "Below" },{ FixFormat::All, "All" } };
	TwType formatType = TwDefineEnum("Fix Format", formatEV, 6);
	TwAddVarRW(msoBar, "Fix Format", formatType, &m_GD->m_MSOManager->m_fixformat, "group=FixParameters key=ObjectType");
	TwAddVarRW(msoBar, "Fix Index", TW_TYPE_INT32, &m_GD->m_MSOManager->m_fixIndex, "min=0.0  max=10000.0  step=1.0 group=FixParameters key=SectionsZ");
	TwEnumVal dimensionEV[] = { { FixDimension::X, "X" },{ FixDimension::Y, "Y" },{ FixDimension::Z, "Z" } };
	TwType dimensionType = TwDefineEnum("Fix Dimension", dimensionEV, 3);
	TwAddVarRW(msoBar, "Fix Dimension", dimensionType, &m_GD->m_MSOManager->m_fixDimension, "group=FixParameters key=ObjectType");

	TwAddVarRW(msoBar, "Save Object", TW_TYPE_BOOL16, &m_saveMSO, "group=Save key=InternalFlag");
}

//Creates MSO cube primitive tool bar
void GUIManager::CreateCubeBar()
{
	msoBar = TwNewBar("MSO Cube Parameters");
	int position[2] = { 240, 20 };
	TwSetParam(msoBar, NULL, "position", TW_PARAM_INT32, 2, &position);

	TwAddVarRW(msoBar, "Spawn Primitive", TW_TYPE_BOOL16, &m_spawnPrimitive, "group=ObjectParameters key=InternalFlag");
	TwAddVarRW(msoBar, "Section Width", TW_TYPE_INT8, &m_GD->m_MSOManager->m_width, "min=1.0  max=100.0  step=1.0 group=ObjectParameters key=SectionWidth");
	TwAddVarRW(msoBar, "Section Height", TW_TYPE_INT8, &m_GD->m_MSOManager->m_height, "min=1.0  max=100.0  step=1.0 group=ObjectParameters key=SectionHeight");
	TwAddVarRW(msoBar, "Section Depth", TW_TYPE_INT8, &m_GD->m_MSOManager->m_depth, "min=1.0  max=100.0  step=1.0 group=ObjectParameters key=SectionDepth");
	TwAddVarRW(msoBar, "Sections X", TW_TYPE_INT8, &m_GD->m_MSOManager->m_sectionsX, "min=1.0  max=20.0  step=1.0 group=ObjectParameters key=SectionsX");
	TwAddVarRW(msoBar, "Sections Y", TW_TYPE_INT8, &m_GD->m_MSOManager->m_sectionsY, "min=1.0  max=20.0  step=1.0 group=ObjectParameters key=SectionsY");
	TwAddVarRW(msoBar, "Sections Z", TW_TYPE_INT8, &m_GD->m_MSOManager->m_sectionsZ, "min=1.0  max=20.0  step=1.0 group=ObjectParameters key=SectionsZ");

	TwEnumVal formatEV[] = { { FixFormat::None, "None" },{ FixFormat::Single, "Single" },{ FixFormat::Level, "Level" },{ FixFormat::Above, "Above" },{ FixFormat::Below, "Below" },{ FixFormat::All, "All" } };
	TwType formatType = TwDefineEnum("Fix Format", formatEV, 6);
	TwAddVarRW(msoBar, "Fix Format", formatType, &m_GD->m_MSOManager->m_fixformat, "group=FixParameters key=ObjectType");
	TwAddVarRW(msoBar, "Fix Index", TW_TYPE_INT32, &m_GD->m_MSOManager->m_fixIndex, "min=0.0  max=10000.0  step=1.0 group=FixParameters key=SectionsZ");
	TwEnumVal dimensionEV[] = { { FixDimension::X, "X" },{ FixDimension::Y, "Y" },{ FixDimension::Z, "Z" } };
	TwType dimensionType = TwDefineEnum("Fix Dimension", dimensionEV, 3);
	TwAddVarRW(msoBar, "Fix Dimension", dimensionType, &m_GD->m_MSOManager->m_fixDimension, "group=FixParameters key=ObjectType");

	TwAddVarRW(msoBar, "Save Object", TW_TYPE_BOOL16, &m_saveMSO, "group=Save key=InternalFlag");
}