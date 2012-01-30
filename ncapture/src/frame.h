
#ifndef __ncapture_frame
#define __ncapture_frame

#include "gui.h"
#include "targetter.h"

namespace ncapture {

class EventReceiver : public Window {
private:
    static EventReceiver *_this;
    static HHOOK kbdHookHandle;
	static HHOOK mouseHookHandle;
	WindowFinder *finder;


public:
	EventReceiver();
	virtual ~EventReceiver();


protected:
	virtual void prepareCreating(CREATESTRUCT *cs);
	virtual LRESULT windowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	static LRESULT CALLBACK mouseHookProc(int code, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK keyboardHookProc(int code, WPARAM wParam, LPARAM lParam);
	void unhook();
	void captureTarget(HWND target);
};

}

#endif // __ncapture_frame
