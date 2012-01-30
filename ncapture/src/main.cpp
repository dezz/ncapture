
#include "precompiled.h"
#include "app.h"
#include "frame.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	int rv = 0;

	try {

		ncapture::Application app;
		ncapture::EventReceiver er;
		er.init();

		rv = app.startMainLoop();

	} catch (const std::exception &ex) {
		MessageBoxA(NULL, ex.what(), NULL, MB_OK);
		rv = 1;
	}

	return rv;
}
