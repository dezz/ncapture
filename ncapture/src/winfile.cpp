
#include "precompiled.h"
#include "winfile.h"

namespace ncapture {

WinFile::WinFile(const tstring &filename, DWORD mode, DWORD creation) : handle(NULL) {
	HANDLE file = ::CreateFile(filename.c_str(), mode, 0, NULL,
		creation, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE) {
		throw NCaptureException("Failed to open file. (WinFile)");
	}
	this->handle = file;
}

WinFile::~WinFile() {
	if (this->handle) {
		::CloseHandle(this->handle);
	}
}

void WinFile::write(LPVOID src, size_t len) {
	BOOL rv;
	DWORD written;
	rv = ::WriteFile(this->handle, src, len, &written, NULL);
	if (!rv || written < (DWORD)len) {
		throw NCaptureException("Failed to write to file.");
	}
}

}

