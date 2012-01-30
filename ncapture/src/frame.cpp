
#include "precompiled.h"
#include "frame.h"
#include "constant.h"
#include "app.h"
#include "capture.h"

namespace ncapture {

EventReceiver *EventReceiver::_this = NULL;
HHOOK EventReceiver::kbdHookHandle = NULL;
HHOOK EventReceiver::mouseHookHandle = NULL;

EventReceiver::EventReceiver() {
	if (_this) {
		throw NCaptureException("EventReciever class is already instantiated.");
	}
    _this = this;

    // hide
//	ShowWindow(this->getHandle(), SW_HIDE);
//	UpdateWindow(this->getHandle());

    finder = new WindowFinder();
	finder->init();

	HMODULE module = ::GetModuleHandle(NULL);

    mouseHookHandle = SetWindowsHookEx(WH_MOUSE_LL, &mouseHookProc, module, 0);
    if (!mouseHookHandle) {
        throw NCaptureException("failed to set mouse hook.");
    }
	
	kbdHookHandle = SetWindowsHookEx(WH_KEYBOARD_LL, &keyboardHookProc, module, 0);
    if (!kbdHookHandle) {
        throw NCaptureException("failed to set keyboard hook.");
    }
}

EventReceiver::~EventReceiver() {
	unhook();

	if (this->finder) {
		delete finder;
	}
}

void EventReceiver::prepareCreating(CREATESTRUCT *cs) {
	Window::prepareCreating(cs);
	cs->style = WS_POPUP;
}

LRESULT EventReceiver::windowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND target;

	switch (uMsg) {
//			break;

		case WM_CLOSE:
			PostQuitMessage(0);
			break;

		case NCAPTURE_CAPTURE_WINDOW:
			unhook();
			finder->eraseTargetMark();
			::RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_ALLCHILDREN | RDW_INTERNALPAINT | RDW_FRAME | RDW_INVALIDATE | RDW_ERASENOW);
			// TODO: how long should i wait for?
			::Sleep(50);
			target = (HWND)lParam;
			// TODO: capture target
			captureTarget(target);
			Application::getInstance()->quit(NCAPTURE_EXIT_SUCCESSFULLY);
			break;

		case NCAPTURE_CANCEL_BY_USER:
			unhook();
			Application::getInstance()->quit(NCAPTURE_EXIT_CANCELLED);
			break;

		default:
			break;
	}
	
	return Window::windowProcedure(hWnd, uMsg, wParam, lParam);
}

void EventReceiver::captureTarget(HWND target) {
	NormalScreenCapture capture;
	capture.capture(target, Application::getInstance()->getOutputFilename());
}

void EventReceiver::unhook() {
	if (mouseHookHandle) {
		if (::UnhookWindowsHookEx(mouseHookHandle)) {
			mouseHookHandle = NULL;
		}
	}
	if (kbdHookHandle) {
		if (::UnhookWindowsHookEx(kbdHookHandle)) {
			kbdHookHandle = NULL;
		}
	}
}

LRESULT CALLBACK EventReceiver::mouseHookProc(int code, WPARAM wParam, LPARAM lParam) {

    if (code < 0) {
        CallNextHookEx(mouseHookHandle, code, wParam, lParam);
    }

	switch (wParam) {
		case WM_MOUSEMOVE: {
			MSLLHOOKSTRUCT *st = (MSLLHOOKSTRUCT *)lParam;
			_this->finder->onMouseMoving(st->pt.x, st->pt.y);
			break;
		}

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
			break;

		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
			return TRUE;

		case WM_RBUTTONUP: {
			HWND target = _this->finder->getCurrentTarget();
			PostMessage(_this->getHandle(), NCAPTURE_CAPTURE_WINDOW, 0, (LPARAM)target);
			return TRUE; // cancel input
			break;
		}
    }

    return CallNextHookEx(mouseHookHandle, code, wParam, lParam);
}

LRESULT CALLBACK EventReceiver::keyboardHookProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code < 0) {
        CallNextHookEx(kbdHookHandle, code, wParam, lParam);
    }

	switch (wParam) {
		case WM_KEYDOWN: {
			LPKBDLLHOOKSTRUCT klhs = (LPKBDLLHOOKSTRUCT)lParam;
			if (klhs->vkCode == VK_ESCAPE) {
				::PostMessage(_this->getHandle(), NCAPTURE_CANCEL_BY_USER, 0, 0);
			}
			return TRUE;
			break;
		}

		default:
			break;
    }

    return CallNextHookEx(kbdHookHandle, code, wParam, lParam);
}

}
