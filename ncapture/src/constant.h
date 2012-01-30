
#ifndef __ncapture_constant
#define __ncapture_constant

namespace ncapture {

enum NCaptureMessage {
	NCAPTURE_CAPTURE_WINDOW = (WM_USER + 101),
	NCAPTURE_CANCEL_BY_USER
};

enum NCaptureExitCode {
	NCAPTURE_EXIT_SUCCESSFULLY = 0,
	NCAPTURE_EXIT_ERRORED = 1,
	NCAPTURE_EXIT_CANCELLED = 2
};


}

#endif // __ncapture_constant
