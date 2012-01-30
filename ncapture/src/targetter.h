
#ifndef __ncapture_targetter
#define __ncapture_targetter

#include "gui.h"

namespace ncapture {

class TargettingFrame : public Window {
private:
	static const int FRAME_WIDTH;
	static const tstring WINDOW_CLASS_NAME;
	HRGN frameRgn;
	HBRUSH backgroundBrush;

public:
    TargettingFrame();
	virtual ~TargettingFrame();
	void showAt(const RECT &rect);
	void hide();

protected:
	virtual const tstring &windowClassName();
	virtual void prepareRegistering(WNDCLASSEX *wce);
	virtual void prepareCreating(CREATESTRUCT *cs);
	virtual LRESULT windowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void setRegionAsFrame();
};

class WindowFinder {
private:
    HWND currentTarget;
	TargettingFrame targettingFrame;
	RECT currentRect;

public:
	WindowFinder();
	void init();
    void onMouseMoving(int x, int y);
    void drawTargetMark(HWND wnd);
    void eraseTargetMark();

private:
	HWND findTopLevelParent(HWND wnd);
	bool compareRect(const RECT &rect1, const RECT &rect2) const;

public:
	HWND getCurrentTarget() const;
};

}

#endif // __ncapture_targetter
