#include "Image.h"

using namespace WKGL;

Image * WKGL::Image::loadFromFile(const char * fileName)
{
	FREE_IMAGE_FORMAT fmt = FreeImage_GetFileType(fileName, 0);
	if (fmt == FIF_UNKNOWN)
	{
		return NULL;
	}

	FIBITMAP * bmap = FreeImage_Load(fmt, fileName, 0);
	FIBITMAP * tmp = bmap;
	bmap = FreeImage_ConvertTo32Bits(bmap);
	FreeImage_Unload(tmp);

	BYTE * buf = FreeImage_GetBits(bmap);
	int width = FreeImage_GetWidth(bmap);
	int height = FreeImage_GetHeight(bmap);

	// 因为image和win坐标系y轴相反,将图片y轴翻转
	int     pitch = width * 4;
	BYTE*   row = new BYTE[width * 4];
	for (int j = 0; j < height / 2; j++)
	{
		memcpy(row, buf + j * pitch, pitch);

		memcpy(buf + j * pitch, buf + (height - j - 1) * pitch, pitch);

		memcpy(buf + (height - j - 1) * pitch, row, pitch);

	}
	delete[]row;

	Image * img = new Image(width, height, (void *)buf);

	FreeImage_Unload(bmap);
	return img;
}

WKGL::Image::Image(int w, int h, void * buf /*= NULL */) : _width(w)
, _height(h)
{
	if (w == 0 || h == 0 || buf == NULL)
	{
		_width = 0;
		_height = 0;
		_pixel = NULL;
	}
	else
	{
		_pixel = new uint[w * h];
		memcpy(_pixel, buf, w * h * sizeof(uint));
	}
}

WKGL::Image::~Image()
{
	if (_pixel != NULL)
	{
		delete[] _pixel;
		_pixel = NULL;
	}
}
