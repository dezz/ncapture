
#ifndef __ncapture_gui
#define __ncapture_gui

#include "basictypes.h"

namespace ncapture {

class Window {
private:
	static const tstring DEFAULT_WINDOW_CLASSNAME;
	static const tstring DEFAULT_WINDOW_NAME;
	static std::map<HWND, Window *> objectMap;
	HWND _handle;

protected:
	virtual const tstring &windowClassName();
	virtual void prepareRegistering(WNDCLASSEX *wce);
	virtual void prepareCreating(CREATESTRUCT *cs);
	virtual LRESULT windowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	Window();
	virtual void init();
	virtual ~Window();

	static LRESULT CALLBACK staticWindowProcedure(
			HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	HWND getHandle() const;
};

}

#endif // __ncapture_gui
