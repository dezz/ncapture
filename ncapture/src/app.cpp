
#include "precompiled.h"
#include "app.h"
#include "basictypes.h"

namespace ncapture {

Application *Application::_this = NULL;

Application::Application() {
	if (_this) {
		throw NCaptureException("Application is already instantiated.");
	}

	parseCommandLine(&this->args);
	if (this->args.size() < REQUIRED_ARG_COUNT) {
		throw NCaptureException("Too few arguments. Please specify filename.");
	}
	_this = this;
}

void Application::parseCommandLine(std::vector<tstring> *args) {
	LPCTSTR str = ::GetCommandLine();
	tstring buf;
	while (TCHAR c = *str) {
		if (c == (TCHAR)' ' || c == (TCHAR)'\t') {
			if (buf.length()) {
				args->push_back(buf);
			}
			buf.clear();
		} else {
			buf.append(1, c);
		}
		str++;
	}
	if (buf.length()) {
		args->push_back(buf);
	}
}

int Application::startMainLoop() {
	MSG msg;

	while (::GetMessage(&msg, NULL, 0, 0 )) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

void Application::quit(int code) {
	::PostQuitMessage(code);
}

Application *Application::getInstance() {
	return _this;
}

const tstring &Application::getOutputFilename() const {
	return this->args[NCAPTURE_ARG_INDEX_FILENAME];
}

}
