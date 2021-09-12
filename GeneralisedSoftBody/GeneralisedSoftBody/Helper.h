//=================================================================
//Helper function to convert to odd char type used by Microsoft stuff
//=================================================================

#ifndef _HELPER_H_
#define _HELPER_H_
#include <string>

struct Helper
{
	static wchar_t* charToWChar(const char* _text)
	{
		size_t size = strlen(_text) + 1;
		static wchar_t* wa = NULL;
		if (wa)
		{
			delete[] wa;
			wa = NULL;
		}
		wa = new wchar_t[size];
		size_t outSize;
		mbstowcs_s(&outSize, wa, size, _text, _TRUNCATE);
		return wa;
	}
};
#endif