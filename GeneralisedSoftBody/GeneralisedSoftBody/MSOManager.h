//=================================================================
// Mass/spring object manager
//=================================================================

#ifndef _MSO_MANAGER_H_
#define _MSO_MANAGER_H_
#include <string>
#include "MSOTypes.h"
#include "FixDimension.h"
#include "FixFormat.h"

struct GlobalData;
struct DrawData3D;

using namespace std;

class MSOManager
{
public:
	MSOManager(GlobalData* _GD);
	~MSOManager();

	void CreateMSOPrimitive(GlobalData* _GD);
	void CreateMSOFile(const char* _fileName, std::string _name, GlobalData* _GD);
	void CreateMSOFBX(const char* _fileName, std::string _name, GlobalData* _GD);
	void SaveMSO(GlobalData* _GD);

	MSOTypes m_currentType;

	bool m_internal = true;

	//GUI MSO Values
	int m_width = 10;
	int m_height = 10;
	int m_depth = 10;

	int m_sectionsX = 3;
	int m_sectionsY = 3;
	int m_sectionsZ = 3;

	int m_fixIndex = 0;
	FixFormat m_fixformat = FixFormat::Level;
	FixDimension m_fixDimension = FixDimension::Y;

	bool m_pauseObject = false;
	bool m_resetVertices = false;

private:
	void DeleteMSO(GlobalData* _GD);

	//Holds the previous MSO primitive type
	MSOTypes m_prevType;

	//Holds the name of current MSO for saving and deleing functions
	string m_currentMSOName;
};

#endif;