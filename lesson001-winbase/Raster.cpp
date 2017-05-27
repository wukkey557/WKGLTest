#include "Raster.h"

WKGL::Raster::Raster(int width, int height, void * pBuffer, Rgba color) : _width(width)
, _height(height)
, _buf((Rgba *)pBuffer)
, _color(color)
{
	//_buf = new Rgba[width * height];
	clear();
}

WKGL::Raster::Raster()
{

}

WKGL::Raster::~Raster()
{
	/*if ( _buf )
			{
			delete[] _buf;
			_buf = NULL;
			}*/
}

void WKGL::Raster::clear()
{
	memset(_buf, 0, _width * _height * sizeof(Rgba));
}

void WKGL::Raster::drawPoint(int posX, int posY, Rgba color, unsigned ptSize)
{
	switch (ptSize)
	{
	case 1:
	setPixel(posX, posY, color);
	break;
	case 2:
	for (int i = 0; i <= 1; i++)
	{
		for (int j = 0; j <= 1; j++)
		{
			setPixel(posX + j, posY + i, color);
		}
	}
	break;
	case 3:
	for (int i = 0; i <= 2; i++)
	{
		for (int j = 0; j <= 2; j++)
		{
			setPixel(posX + j, posY + i, color);
		}
	}
	break;
	default:
	break;
	}
}

void WKGL::Raster::drawPoint(float2 pos, Rgba color)
{
	drawPoint(pos.x, pos.y, color, 1);
}

void WKGL::Raster::drawLine( float2 posSrc, float2 posDst, Rgba color1, Rgba color2 )
{
	float offsetX = posDst.x - posSrc.x;
	float offsetY = posDst.y - posSrc.y;
	float2 startPos = posSrc;
	float2 endPos = posDst;
	if (fabs(offsetX) >= fabs(offsetY))
	{
		if (posSrc.x > posDst.x)
		{
			startPos = posDst;
			endPos = posSrc;
		}

		if (offsetX != 0)
		{
			float slope = getSlope(posSrc, posDst);
			for (float i = startPos.x; i <= endPos.x; i += 1.0f)
			{
				Rgba col = colorLerp( color1, color2, fabs( (i - startPos.x) / offsetX ) );
				setPixel(i, startPos.y + (i - startPos.x) * slope, col);
			}
		}
		else
		{
			for (float i = startPos.y; i <= endPos.y; i += 1.0f)
			{
				Rgba col = color1;
				if ( offsetY != 0 )
				{
					col = colorLerp( color1, color2, fabs( (i - startPos.y) / offsetY ) );
				}
				
				setPixel(startPos.x, i, col);
			}
		}
	}
	else
	{
		if (posSrc.y > posDst.y)
		{
			startPos = posDst;
			endPos = posSrc;
		}

		if (offsetY != 0)
		{
			float slope = getCotSlope(posSrc, posDst);
			for (float i = startPos.y; i <= endPos.y; i += 1.0f)
			{
				Rgba col = colorLerp( color1, color2, fabs( (i - startPos.y) / offsetY ) );
				setPixel((i - startPos.y) * slope + startPos.x, i, col);
			}
		}
		else
		{
			for (float i = startPos.x; i <= endPos.x; i += 1.0f)
			{
				Rgba col = color1;
				if (offsetX != 0)
				{
					col = colorLerp(color1, color2, fabs((i - startPos.x) / offsetX));
				}
				setPixel(i, startPos.y, col);
			}
		}
	}
}

float WKGL::Raster::getSlope(float2 src, float2 dst)
{
	assert(src.x != dst.x);
	float offsetX = dst.x - src.x;
	float offsetY = dst.y - src.y;
	return offsetY / offsetX;
}

float WKGL::Raster::getCotSlope(float2 src, float2 dst)
{
	assert(src.y != dst.y);
	float offsetX = dst.x - src.x;
	float offsetY = dst.y - src.y;
	return offsetX / offsetY;
}

void WKGL::Raster::drawArrays(DRAWMODE mode, const float2 * pArr, int num)
{
	switch (mode)
	{
	case DM_POINTS:
	{
					  for (int i = 0; i < num; i++)
					  {
						  drawPoint(pArr[i], _color);
					  }
	}
	break;
	case DM_LINES:
	{
					 num = num / 2 * 2;
					 for (int i = 0; i < num; i += 2)
					 {
						 drawLine(pArr[i], pArr[i + 1], _color, _color);
					 }
	}
	break;
	case DM_LINE_LOOP:
	{
						 drawLine(pArr[0], pArr[1], _color, _color);
						 for (int i = 1; i < num - 1; i++)
						 {
							 drawLine(pArr[i], pArr[i + 1], _color, _color);
						 }
						 drawLine(pArr[num - 1], pArr[0], _color, _color);
	}
	break;
	case WKGL::Raster::DM_LINE_STRIP:
	{
										for (int i = 0; i < num - 1; i++)
										{
											drawLine(pArr[i], pArr[i + 1], _color, _color);
										}
	}
	break;
	default:
	break;
	}
}

void WKGL::Raster::drawImageAlphaTest(int x, int y, const Image * img, BYTE alpha)
{
	int left = tmax<int>(x, 0);
	int top = tmax<int>(y, 0);

	int right = tmin<int>(x + img->getWidth(), _width);
	int bottom = tmin<int>(y + img->getHeight(), _height);

	for (int j = top; j < bottom; j++)
	{
		for (int i = left; i < right; i++)
		{
			Rgba color = img->pixelAt(i - left, j - top);
			if (color._a < alpha)
			{
				setPixelEx(i, j, color);
			}
		}
	}
}

void WKGL::Raster::drawImageWithColorKey(int x, int y, const Image * img, const Rgba& key)
{
	int left = tmax<int>(x, 0);
	int top = tmax<int>(y, 0);

	int right = tmin<int>(x + img->getWidth(), _width);
	int bottom = tmin<int>(y + img->getHeight(), _height);

	for (int j = top; j < bottom; j++)
	{
		for (int i = left; i < right; i++)
		{
			Rgba color = img->pixelAt(i - left, j - top);
			if (color != key)
			{
				setPixelEx(i, j, color);
			}
		}
	}
}

void WKGL::Raster::drawImage(int x, int y, const Image * img)
{
	int left = tmax<int>(x, 0);
	int top = tmax<int>(y, 0);

	int right = tmin<int>(x + img->getWidth(), _width);
	int bottom = tmin<int>(y + img->getHeight(), _height);

	for (int j = top; j < bottom; j++)
	{
		for (int i = left; i < right; i++)
		{
			Rgba color = img->pixelAt(i - left, j - top);
			setPixelEx(i, j, color);
		}
	}
}

void WKGL::Raster::drawImage(int startX, int startY, const Image * img, int x, int y, int w, int h)
{
	int left = tmax<int>(startX, 0);
	int top = tmax<int>(startY, 0);

	int tmpWidth = tmin<int>(img->getWidth(), w);
	int tmpHeight = tmin<int>(img->getHeight(), h);

	int right = tmin<int>(startX + tmpWidth, _width);
	int bottom = tmin<int>(startY + tmpHeight, _height);

	for (int j = top; j < bottom; j++)
	{
		for (int i = left; i < right; i++)
		{
			// src image color
			Rgba color = img->pixelAt(i - left + x, j - top + y);

			setPixelEx(i, j, color);
		}
	}
}

void WKGL::Raster::drawImage(int x, int y, int w, int h)
{
	int left = tmax<int>(x, 0);
	int top = tmax<int>(y, 0);

	int right = tmin<int>(x + w, _width);
	int bottom = tmin<int>(y + h, _height);

	for (int j = top; j < bottom; j++)
	{
		for (int i = left; i < right; i++)
		{
			Rgba color = Rgba(rand() % 256, rand() % 256, rand() % 256);
			setPixelEx(i, j, color);
		}
	}
}

void WKGL::Raster::drawImageAlphaBlend(int x, int y, const Image * img, float alpha)
{
	int left = tmax<int>(x, 0);
	int top = tmax<int>(y, 0);

	int right = tmin<int>(x + img->getWidth(), _width);
	int bottom = tmin<int>(y + img->getHeight(), _height);

	for (int j = top; j < bottom; j++)
	{
		for (int i = left; i < right; i++)
		{
			// src image color
			Rgba color = img->pixelAt(i - left, j - top);
			// bg image color
			Rgba bgColor = getPixel(i, j);

			Rgba dstColor = colorLerp(bgColor, color, color._a / 255.0f * alpha);
			setPixelEx(i, j, dstColor);
		}
	}
}

void WKGL::Raster::drawImageAlpha(int x, int y, const Image * img, float alpha /*= 1.0f*/)
{
	int left = tmax<int>(x, 0);
	int top = tmax<int>(y, 0);

	int right = tmin<int>(x + img->getWidth(), _width);
	int bottom = tmin<int>(y + img->getHeight(), _height);

	for (int j = top; j < bottom; j++)
	{
		for (int i = left; i < right; i++)
		{
			// src image color
			Rgba color = img->pixelAt(i - left, j - top);
			// bg image color
			Rgba bgColor = getPixel(i, j);

			Rgba dstColor = colorLerp(bgColor, color, alpha);
			setPixelEx(i, j, dstColor);
		}
	}
}

void WKGL::Raster::drawImageScale(int destX, int destY, int destW, int destH, const Image * img)
{
	float scaleW = (float)img->getWidth() / destW;
	float scaleH = (float)img->getHeight() / destH;

	for (int x = destX; x < destX + destW; x++)
	{
		for (int y = destY; y < destY + destH; y++)
		{
			// 对应原始图片上的坐标
			float oldX = (x - destX) * scaleW;
			float oldY = (y - destY) * scaleH;

			Rgba color = img->pixelAt(oldX, oldY);

			/*float oldX = (x - destX) * scaleW;
			float oldY = (y - destY) * scaleH;

			int x1 = tmax<int>((int)oldX, 0);
			int x2 = tmin<int>(x1 + 1, img->getWidth());

			int y1 = tmax<int>((int)oldY, 0);
			int y2 = tmin<int>(y1 + 1, img->getHeight());

			Rgba color1 = img->pixelAt(x1, y1);
			Rgba color2 = img->pixelAt(x2, y2);

			Rgba color = colorLerp(color1, color2, scaleW);*/

			setPixelEx(x, y, color);
		}
	}
}

void WKGL::Raster::drawTriangle(const int2 * points, const Rgba * colors, int count /*= 3 */)
{
	if (!isInRect(points[0]) && !isInRect(points[1]) && !isInRect(points[2]))
	{
		return;
	}

	Ege eges[] =
	{
		Ege(points[0], points[1], colors[0], colors[1]),
		Ege(points[0], points[2], colors[0], colors[2]),
		Ege(points[2], points[1], colors[2], colors[1]),
	};

	int iLong = 0;
	int length = eges[0]._y2 - eges[0]._y1;
	for (int i = 1; i < 3; i++)
	{
		int len = eges[i]._y2 - eges[i]._y1;
		if (len > length)
		{
			length = len;
			iLong = i;
		}
	}

	int iShort1 = (iLong + 1) % 3;
	int iShort2 = (iLong + 2) % 3;

	drawEge(eges[iLong], eges[iShort1]);
	drawEge(eges[iLong], eges[iShort2]);
}

void WKGL::Raster::drawRect(const int2 * points, const Rgba * colors)
{
	int left = tmax<int>(points[0].x, 0);
	int top = tmax<int>(points[0].y, 0);

	int right = tmin<int>(points[2].x, _width);
	int bottom = tmin<int>(points[2].y, _height);

	for (int i = left; i < right; i++)
	{
		float s = 0;
		if (right - left != 0)
			s = fabs((i - left) * 1.0f / (right - left));
		Rgba col1 = colorLerp(colors[0], colors[1], s);
		Rgba col2 = colorLerp(colors[3], colors[2], s);

		for (int j = top; j < bottom; j++)
		{
			s = 0;
			if (bottom - top != 0)
				s = fabs((j - top) * 1.0f / (bottom - top));
			Rgba col = colorLerp(col1, col2, s);
			setPixel(i, j, col);
		}
	}
}

void WKGL::Raster::drawFilledRect(int x, int y, int w, int h, Rgba color /*= Rgba(255, 0, 0) */)
{
	// win窗口下左上角取最大,右下角取最小
	int left = tmax<int>(x, 0);
	int top = tmax<int>(y, 0);

	int right = tmin<int>(x + w, _width);
	int bottom = tmin<int>(y + h, _height);

	for (int j = top; j < bottom; j++)
	{
		for (int i = left; i < right; i++)
		{
			setPixelEx(i, j, color);
		}
	}
}

void WKGL::Raster::drawEge(const Ege& e1, const Ege& e2)
{
	float xOffset1 = e1._x2 - e1._x1;
	float yOffset1 = e1._y2 - e1._y1;
	float xOffset2 = e2._x2 - e2._x1;
	float yOffset2 = e2._y2 - e2._y1;

	if (yOffset1 == 0 || yOffset2 == 0)
		return;

	// 利用了e1的_y1始终不变,e2的y1变化的特点,兼容上下三角形
	float scale1 = (e2._y1 - e1._y1) * 1.0f / yOffset1;
	float step1 = 1.0f / yOffset1;

	float scale2 = 0;
	float step2 = 1.0f / yOffset2;

	int startY2 = tmax<int>(0, e2._y1);
	int endY = tmin<int>(_height, e2._y2);

	int startY1 = tmax<int>(0, e1._y1);

	float s1 = (startY1 - e1._y1) / yOffset1;
	float s2 = (startY2 - e2._y1) / yOffset2;
	scale1 += s1;
	scale2 += s2;

	for (int y = startY2; y < endY; y++)
	{
		int x1 = e1._x1 + scale1 * xOffset1;
		int x2 = e2._x1 + scale2 * xOffset2;

		Rgba color1 = colorLerp(e1._color1, e1._color2, scale1);
		Rgba color2 = colorLerp(e2._color1, e2._color2, scale2);

		InnerLine line(x1, x2, y, color1, color2);
		drawInnerLine(line);

		scale1 += step1;
		scale2 += step2;
	}
}

bool WKGL::Raster::isInRect(const int2& p)
{
	if (p.x >= 0 && p.y >= 0 && p.x <= _width && p.y <= _height)
	{
		return true;
	}
	return false;
}

void WKGL::Raster::drawInnerLine(const InnerLine& line)
{
	float scale = 0;
	int startX = tmax<int>(0, line._startX);
	int endX = tmin<int>(_width, line._endX);
	float deltaX = static_cast<float>(tmax<int>(endX - startX, 1));
	float step = 1.0f / deltaX;

	for (int i = startX; i < endX; i++)
	{
		Rgba col = colorLerp(line._colorStart, line._colorEnd
			, scale);
		setPixelEx(i, line._y, col);
		scale += step;
	}
}

WKGL::Ege::Ege(const int2& p1, const int2& p2, const Rgba& color1 /*= Rgba(255, 0, 0)*/, const Rgba& color2 /*= Rgba(255, 0, 0) */)
{
	if (p1.y <= p2.y)
	{
		_x1 = p1.x;
		_y1 = p1.y;
		_x2 = p2.x;
		_y2 = p2.y;
		_color1 = color1;
		_color2 = color2;
	}
	else
	{
		_x1 = p2.x;
		_y1 = p2.y;
		_x2 = p1.x;
		_y2 = p1.y;
		_color1 = color2;
		_color2 = color1;
	}
}

WKGL::InnerLine::InnerLine(int startX, int endX, int y, Rgba color1 /*= Rgba(255, 0, 0) */, Rgba color2 /*= Rgba(255, 0, 0) */) : _y(y)
{
	if (endX >= startX)
	{
		_startX = startX;
		_endX = endX;
		_colorStart = color1;
		_colorEnd = color2;
	}
	else
	{
		_startX = endX;
		_endX = startX;
		_colorStart = color2;
		_colorEnd = color1;
	}
}
