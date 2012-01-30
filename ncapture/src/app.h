
#ifndef __ncapture_app
#define __ncapture_app

#include "basictypes.h"

namespace ncapture {

class Application {
private:
	static const int REQUIRED_ARG_COUNT = 2;
	static Application *_this;
	std::vector<tstring> args;

	void parseCommandLine(std::vector<tstring> *args);

public:
	enum ArgIndex {
		NCAPTURE_ARG_INDEX_PROGRAM_NAME,
		NCAPTURE_ARG_INDEX_FILENAME
	};

	Application();
	int startMainLoop();
	void quit(int code);
	static Application *getInstance();

	const tstring &getOutputFilename() const;
};

}

#endif // __ncapture_app
