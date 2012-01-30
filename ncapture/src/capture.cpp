
#include "precompiled.h"
#include "capture.h"
#include "winfile.h"

namespace ncapture {

Bitmap::Bitmap(int width, int height, int bitCount)
		: width(width), height(height), bitCount(bitCount) {
	BITMAPINFO &bi = this->bitmapInfo;
	ZeroMemory(&bi, sizeof(BITMAPINFO));

	LPBITMAPINFOHEADER bih = &bi.bmiHeader;
	bih->biSize = sizeof(BITMAPINFOHEADER);
	bih->biWidth = width;
	bih->biHeight = -height;
	bih->biPlanes = 1;
	bih->biBitCount = bitCount;
	bih->biCompression = BI_RGB;
	bih->biSizeImage = 0;
	bih->biXPelsPerMeter = 0;
	bih->biYPelsPerMeter = 0;
	bih->biClrImportant = 0;

	HBITMAP hbmp = ::CreateDIBSection(NULL, &bi, DIB_RGB_COLORS,
			(void **)&this->bmpPixels, NULL, 0);
	if (!hbmp) {
		throw NCaptureException("Failed to create bitmap image on memory.");
	}

	this->handle = hbmp;
}

Bitmap::~Bitmap() {
	if (this->handle) {
		::DeleteObject(this->handle);
	}
}

HBITMAP Bitmap::getHandle() const {
	return this->handle;
}

size_t Bitmap::getSizePerLine() const {
	return adjustAlignment(this->width * (this->bitCount / 8), 4);
}

int Bitmap::getWidth() const {
	return this->width;
}

int Bitmap::getHeight() const {
	return this->height;
}

const BYTE * const Bitmap::getConstRawPixels() const {
	return this->bmpPixels;
}

void Bitmap::save(const tstring &filename) {
	// calcurate file size
	size_t bmpSize = getSizePerLine() * this->height;

	BITMAPFILEHEADER bfh;
	ZeroMemory(&bfh, sizeof(BITMAPFILEHEADER));
	bfh.bfType = 0x4D42; // "BM"
	bfh.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpSize);
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	WinFile file(filename, GENERIC_WRITE, CREATE_ALWAYS);
	file.write(&bfh, sizeof(BITMAPFILEHEADER));
	file.write(&this->bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));
	file.write(this->bmpPixels, bmpSize);
}

size_t Bitmap::adjustAlignment(size_t size, size_t alignment) const {
	size_t remainder = size % alignment;
	if (remainder) {
		return size + (alignment - remainder);
	} else {
		return size;
	}
}

const tstring NormalScreenCapture::PNG_EXTENSION = _T(".png");

NormalScreenCapture::NormalScreenCapture() {
}

void NormalScreenCapture::capture(HWND target, const tstring &filename) {

	struct GDIResource {
		HDC hdc;
		HDC bmpDc;
		HGDIOBJ oldObj;

		GDIResource(HBITMAP bmpHandle) {
			try {
				hdc = ::GetDC(NULL);
				if (!hdc) {
					throw NCaptureException("Failed to get dc from desktop(NULL).");
				}

				bmpDc = ::CreateCompatibleDC(hdc);
				if (!bmpDc) {
					throw NCaptureException("Failed to create compatible dc for bitmap.");
				}

				oldObj = ::SelectObject(bmpDc, bmpHandle);
				if (!oldObj) {
					throw NCaptureException("Failed to select bitmap object onto dc.");
				}
			} catch (const std::exception &) {
				dispose();
				throw;
			}
		}

		void dispose() {
			if (oldObj) {
				::SelectObject(bmpDc, oldObj);
			}
			if (bmpDc) {
				::DeleteObject(bmpDc);
			}
			if (hdc) {
				::ReleaseDC(NULL, hdc);
			}
		}

		~GDIResource() {
			dispose();
		}

	};

	RECT rect;
	if (!::GetWindowRect(target, &rect)) {
		throw NCaptureException("Failed to get target window rectangle.");
	}

	int width, height;
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	Bitmap bmp(width, height);
	GDIResource resource(bmp.getHandle());

	if (!::BitBlt(resource.bmpDc, 0, 0, width, height, resource.hdc, rect.left, rect.top, SRCCOPY)) {
		throw NCaptureException("Failed to copy window screen image to bitmap.");
	}

	int dosave = IDYES;
	if (boost::filesystem::exists(filename)) {
		// TODO: move it to resource file, or something like.
		std::basic_stringstream<TCHAR> msg;
		msg << _T("Filename '") << filename << _T("' already exists.") << std::endl;
		msg << _T("Are you sure you want to overwrite ?") << std::endl;
		dosave = MessageBox(NULL, msg.str().c_str(), _T("Confirm"), MB_YESNO | MB_ICONEXCLAMATION);
	}

	if (dosave == IDYES) {
#ifdef UNICODE
		boost::filesystem::wpath filepath(filename);
#else
		boost::filesystem::path filepath(filename);
#endif // UNICODE
		if (_tcsicmp(filepath.extension().c_str(), PNG_EXTENSION.c_str()) == 0) {
			PNGFileWriter writer(filename);
			writer.init();
			writer.write(&bmp);
			writer.writeEnd();
		} else {
			bmp.save(filename);
		}
	}
}

PNGFileWriter::PNGFileWriter(const tstring &filename) {
	FILE *fp = NULL;
	png_structp pngWriteStruct = NULL;
	png_infop pngInfoStruct = NULL;

	try {
		fp = _tfopen(filename.c_str(), _T("wb"));
		if (!fp) {
			throw NCaptureException("Failed to open file for writing png image.");
		}

		// prepare png write struct
		png_structp pngWriteStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!pngWriteStruct) {
			throw NCaptureException("Failed to call 'png_create_write_struct'.");
		}

		// prepare png info struct
		png_infop pngInfoStruct = png_create_info_struct(pngWriteStruct);
		if (!pngInfoStruct) {
			throw NCaptureException("Failed to call 'png_create_info_struct'.");
		}

		this->file = fp;
		this->png = pngWriteStruct;
		this->pngInfo = pngInfoStruct;

	} catch (const std::exception &e) {
		if (pngWriteStruct) {
			if (pngInfoStruct) {
				png_destroy_write_struct(&pngWriteStruct, &pngInfoStruct);
			} else {
				png_destroy_write_struct(&pngWriteStruct, (png_infopp)NULL);
			}
		}
		if (fp) {
			fclose(fp);
		}
		
		throw;
	}
}

PNGFileWriter::~PNGFileWriter() {
	if (setjmp(png_jmpbuf(this->png))) {
		// ignore errors
	}
	png_destroy_write_struct(&this->png, &this->pngInfo);
	fclose(this->file);
}

void PNGFileWriter::init() {
	if (setjmp(png_jmpbuf(this->png))) {
		throw NCaptureException("An error occurred when preparing.");
	}

	png_init_io(this->png, this->file);
}

void PNGFileWriter::write(const Bitmap *bmp) {
	int width = bmp->getWidth();
	int height = bmp->getHeight();
	const BYTE * const bmpPixels = bmp->getConstRawPixels();
	size_t sizePerLine = bmp->getSizePerLine();

	if (setjmp(png_jmpbuf(this->png))) {
		throw NCaptureException("An error occurred when writing png image.");
	}

	png_set_IHDR(this->png, this->pngInfo, width, height,
			8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_BASE);
	png_write_info(this->png, this->pngInfo);

	png_byte *row = new png_byte[sizePerLine];
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			png_byte *dest = row + (x * 3);
			byte const *src = bmpPixels + sizePerLine * y + (x * 3);

			// reverse r, g, b order,
			// because Win32API 'CreateDIBSection' locate 'b, g, r'.
			// maybe it was caused Intel x86 architecture.
			byte b = *src++;
			byte g = *src++;
			byte r = *src++;

			// R
			*dest++ = r;
			// G
			*dest++ = g;
			// B
			*dest++ = b;
		}

		png_write_row(this->png, row);
	}

	delete[] row;
}

void PNGFileWriter::writeEnd() {
	if (setjmp(png_jmpbuf(this->png))) {
		throw NCaptureException("An error occurred when writing png image (end).");
	}
	png_write_end(this->png, this->pngInfo);
}

}
