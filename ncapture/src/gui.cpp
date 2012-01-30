
#include "precompiled.h"
#include "gui.h"

namespace ncapture {

const tstring Window::DEFAULT_WINDOW_CLASSNAME = _T("ncapture default window class");
const tstring Window::DEFAULT_WINDOW_NAME = _T("ncapture");
std::map<HWND, Window *> Window::objectMap;

Window::Window() {
}

void Window::init() {
	WNDCLASSEX wce;
	if (!GetClassInfoEx(GetModuleHandle(NULL),
			windowClassName().c_str(), &wce)) {
		prepareRegistering(&wce);
		if (!RegisterClassEx(&wce)) {
			throw NCaptureException("failed to register window class.");
		}
	}

	CREATESTRUCT cs;
	prepareCreating(&cs);
    HWND hwnd = CreateWindowEx(
        cs.dwExStyle,
        cs.lpszClass,
        cs.lpszName,
        cs.style,
        cs.x,
        cs.y,
        cs.cx,
        cs.cy,
        cs.hwndParent,
        cs.hMenu,
        cs.hInstance,
        (LPVOID)this);
    if (!hwnd) {
        throw NCaptureException("failed to create window.");
    }

}

Window::~Window() {
}

const tstring &Window::windowClassName() {
	return DEFAULT_WINDOW_CLASSNAME;
}

void Window::prepareRegistering(WNDCLASSEX *wce) {
	wce->cbSize = sizeof(WNDCLASSEX);
    wce->style = CS_HREDRAW | CS_VREDRAW;
	wce->lpfnWndProc = &Window::staticWindowProcedure;
    wce->cbClsExtra = 0;
    wce->cbWndExtra = 0;
    wce->hInstance = GetModuleHandle(NULL);
    wce->hIcon = NULL;
    wce->hCursor = LoadCursor(NULL, IDC_ARROW);
    wce->hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wce->lpszMenuName = NULL;
	wce->lpszClassName = DEFAULT_WINDOW_CLASSNAME.c_str();
	wce->hIconSm = NULL;
}

void Window::prepareCreating(CREATESTRUCT *cs) {
	cs->hInstance = GetModuleHandle(NULL);
	cs->hMenu = NULL;
	cs->hwndParent = NULL;
	cs->x = 100;
	cs->y = 100;
	cs->cx = 400;
	cs->cy = 300;
	cs->lpszName = DEFAULT_WINDOW_NAME.c_str();
	cs->lpszClass = DEFAULT_WINDOW_CLASSNAME.c_str();
	cs->style = WS_OVERLAPPEDWINDOW;
	cs->dwExStyle = 0;

}

LRESULT Window::windowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK Window::staticWindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	Window *obj;
	if (uMsg == WM_NCCREATE) {
		LPCREATESTRUCT cs = (LPCREATESTRUCT)lParam;
		obj = (Window *)cs->lpCreateParams;
		objectMap.insert(std::make_pair(hWnd, obj));
		obj->_handle = hWnd;
	} else {
		std::map<HWND, Window *>::const_iterator it = objectMap.find(hWnd);
		if (it == Window::objectMap.end()) {
			// WM_GETMINMAXINFO cannot be cought by windowProcedure
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		} else {
			obj = objectMap[hWnd];
		}
	}
	return obj->windowProcedure(hWnd, uMsg, wParam, lParam);
}

HWND Window::getHandle() const {
	return this->_handle;
}

}
