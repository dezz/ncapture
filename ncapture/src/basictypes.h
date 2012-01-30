
#ifndef __ncapture_basictypes
#define __ncapture_basictypes

typedef std::basic_string<TCHAR> tstring;

class NCaptureException : public std::exception {
public:
	NCaptureException(const char *msg) : std::exception(msg) {}
	virtual ~NCaptureException() throw() {}
};



#endif // __ncapture_basictypes
