//=================================================================
// Mass/spring MSO Text file object
//=================================================================

#ifndef _MSO_FILE_H_
#define _MSO_FILE_H_
#include "VertexMSO.h"
#include <string>

class MSOFile : public VertexMSO
{
public:
	MSOFile(const char* _fileName, std::string _name, GlobalData* _GD);
	~MSOFile();

	void Initilaise(ID3D11Device* _ID);

	virtual void Tick(GlobalData* _GD) { VertexMSO::Tick(_GD); };
	virtual void Draw(DrawData3D* _DD) { VertexMSO::Draw(_DD); };

private:
	void CreateVertices();

	//MSO Text file name
	const char* m_fileName;
};

#endif;
