
#ifndef __ncapture_capture
#define __ncapture_capture

#include "basictypes.h"

namespace ncapture {

class Bitmap {
private:
	int width;
	int height;
	int bitCount;
	HBITMAP handle;
	BITMAPINFO bitmapInfo;
	BYTE *bmpPixels;

	size_t adjustAlignment(size_t size, size_t alignment) const;

public:
	enum BitCount {
		BITCOUNT_TRUECOLOR = 24,
		BITCOUNT_TRUECOLOR_ALPHA = 32,
	};

	Bitmap(int width, int height, int bitCount = BITCOUNT_TRUECOLOR);
	~Bitmap();
	void save(const tstring &filename);

public:
	HBITMAP getHandle() const;
	size_t getSizePerLine() const;
	int getWidth() const;
	int getHeight() const;
	const BYTE * const getConstRawPixels() const;
};

class ScreenCapture {
public:
	virtual void capture(HWND target, const tstring &filename) = 0;
	virtual ~ScreenCapture();
};

inline ScreenCapture::~ScreenCapture() {}

class NormalScreenCapture : public ScreenCapture {
private:
	static const tstring PNG_EXTENSION;

public:
	NormalScreenCapture();
	virtual void capture(HWND target, const tstring &filename);
};

class PNGFileWriter {
private:
	FILE *file;
	png_structp png;
	png_infop pngInfo;

public:
	PNGFileWriter(const tstring &filename);
	~PNGFileWriter();
	void init();
	void write(const Bitmap *bmp);
	void writeEnd();
};

}

#endif // __ncapture_capture
