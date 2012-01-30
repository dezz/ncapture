
#include "precompiled.h"
#include "targetter.h"

namespace ncapture {

WindowFinder::WindowFinder() {
}

void WindowFinder::init() {
	this->targettingFrame.init();
	POINT pt;
	::GetCursorPos(&pt);
	onMouseMoving(pt.x, pt.y);
}

void WindowFinder::onMouseMoving(int x, int y) {
	POINT pt = {x, y};
	HWND wnd = ::WindowFromPoint(pt);
	wnd = findTopLevelParent(wnd);
    if (wnd != NULL && wnd != this->targettingFrame.getHandle()) {
        // draw line on window frame
		RECT nowRect;
		::GetWindowRect(wnd, &nowRect);
		if (!compareRect(currentRect, nowRect)) {
//        if (currentTarget != wnd) {
            drawTargetMark(wnd);
            this->currentTarget = wnd;
			this->currentRect = nowRect;
        }
    }
}

bool WindowFinder::compareRect(const RECT &rect1, const RECT &rect2) const {
	BYTE *r1 = (BYTE *)&rect1;
	BYTE *r2 = (BYTE *)&rect2;
	BYTE *r1end = r1 + sizeof(RECT);

	for (; r1 < r1end; r1++, r2++) {
		if (*r1 != *r2) {
			return false;
		}
	}
	return true;
}

HWND WindowFinder::findTopLevelParent(HWND wnd) {
	HWND w = wnd;
	HWND parent;

	while ((parent = ::GetParent(w)) != NULL) {
		w = parent;
	}
	return w;
}


void WindowFinder::drawTargetMark(HWND wnd) {
    RECT rect;
	if (!::GetWindowRect(wnd, &rect)) {
		throw NCaptureException("failed to get window rectangle by handle.");
    }
	this->targettingFrame.showAt(rect);
}

void WindowFinder::eraseTargetMark() {
	this->targettingFrame.hide();
}

HWND WindowFinder::getCurrentTarget() const {
	return this->currentTarget;
}


const int TargettingFrame::FRAME_WIDTH = 5;
const tstring TargettingFrame::WINDOW_CLASS_NAME = _T("ncapture targetting frame");

TargettingFrame::TargettingFrame() {
    // hide
	ShowWindow(this->getHandle(), SW_HIDE);
	UpdateWindow(this->getHandle());
}

TargettingFrame::~TargettingFrame() {
	if (this->frameRgn) {
		DeleteObject(this->frameRgn);
	}

	if (this->backgroundBrush) {
		DeleteObject(this->backgroundBrush);
	}
}

const tstring &TargettingFrame::windowClassName() {
	return WINDOW_CLASS_NAME;
}

void TargettingFrame::prepareRegistering(WNDCLASSEX *wce) {
	Window::prepareRegistering(wce);

	HBRUSH brush = CreateSolidBrush(RGB(0xFF, 0x00, 0x00));
	if (!brush) {
		throw NCaptureException("failed to create red brush.");
	}
	this->backgroundBrush = brush;
	wce->hbrBackground = brush;
}

void TargettingFrame::prepareCreating(CREATESTRUCT *cs) {
	Window::prepareCreating(cs);
	cs->style = WS_POPUP;
	cs->dwExStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
}

void TargettingFrame::setRegionAsFrame() {
	RECT rect;
	::GetWindowRect(getHandle(), &rect);
	HRGN frameRgn = ::CreateRectRgn(0, 0, rect.right - rect.left, rect.bottom - rect.top);
	HRGN innerRgn = ::CreateRectRgn(
			FRAME_WIDTH,
			FRAME_WIDTH,
			rect.right - rect.left - FRAME_WIDTH,
			rect.bottom - rect.top - FRAME_WIDTH);
	::CombineRgn(frameRgn, frameRgn, innerRgn, RGN_DIFF);
	::DeleteObject(innerRgn);

	::SetWindowRgn(getHandle(), frameRgn, TRUE);
	HRGN oldRgn = this->frameRgn;
	this->frameRgn = frameRgn;

	DeleteObject(oldRgn);
}

void TargettingFrame::showAt(const RECT &rect) {
	SetWindowPos(getHandle(), NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
	setRegionAsFrame();
	ShowWindow(getHandle(), SW_SHOW);
	UpdateWindow(getHandle());
}

void TargettingFrame::hide() {
	::ShowWindow(getHandle(), SW_HIDE);
//	::UpdateWindow(getHandle());
}

LRESULT TargettingFrame::windowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_MOUSEACTIVATE:
			return MA_NOACTIVATE;
			break;

		default:
			break;
	}

	return Window::windowProcedure(hWnd, uMsg, wParam, lParam);
}

}
