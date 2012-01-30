
#ifndef __ncapture_winfile
#define __ncapture_winfile

#include "basictypes.h"

namespace ncapture {

class WinFile {
private:
	HANDLE handle;

public:
	WinFile(const tstring &filename, DWORD mode, DWORD creation);
	~WinFile();
	void write(LPVOID src, size_t len);
};

}

#endif // __ncapture_winfile
