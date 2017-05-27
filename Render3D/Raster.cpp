#include "Raster.h"

WKGL::Raster::Raster(int width, int height, void * pBuffer, Rgba color) : _width(width)
, _height(height)
, _buf((Rgba *)pBuffer)
, _color(color)
, _image(NULL)
{
	//_buf = new Rgba[width * height];
	clear();

	memset( &_positionPointer, 0, sizeof(DataElementDes) );
	memset( &_colorPointer, 0, sizeof(DataElementDes) );
	memset( &_uvPointer, 0, sizeof(DataElementDes) );

	// 未指定颜色指针时默认值
	int size = sizeof(_defColArr) / sizeof(Rgba);
	for (int i = 0; i < size; i++) 
	{
		_defColArr[i] = Rgba(0, 0, 0);
		_defUVArr[i] = float2(0, 0);
	}
		
	_defColPointer._size = 4;
	_defColPointer._type = DT_BYTE;
	_defColPointer._stride = sizeof(Rgba);
	_defColPointer._data = _defColArr;

	_defUVPointer._size = 2;
	_defUVPointer._type = DT_FLOAT;
	_defUVPointer._stride = sizeof(float2);
	_defUVPointer._data = _defUVArr;

	_matModel = matrix4(1);	// 默认为单位矩阵
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



void WKGL::Raster::drawEge1(const Ege& e1, const Ege& e2)
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

		float2 uv1 = uvLerp( e1._uv1, e1._uv2, scale1 );
		float2 uv2 = uvLerp( e2._uv1, e2._uv2, scale2 );

		InnerLine line(x1, x2, y, uv1, uv2, color1, color2);
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
		/*Rgba col = colorLerp(line._colorStart, line._colorEnd
			, scale);*/

		float2 uv = uvLerp( line._uvStart, line._uvEnd, scale );
		
		Rgba col;
		if ( _image ) 
		{
			col = _image->pixelUVAt(uv);
			col = col + line._colorStart;
		}
		else 
		{
			col = line._colorStart;
		}

		setPixel(i, line._y, col);
		//setPixelEx(i, line._y, col);
		scale += step;
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

WKGL::Ege::Ege(const int2& p1, const int2& p2
	, const float2& uv1, const float2& uv2
	, const Rgba& color1 /*= Rgba(255, 0, 0)*/, const Rgba& color2 /*= Rgba(255, 0, 0) */)
{
	if (p1.y <= p2.y)
	{
		_x1 = p1.x;
		_y1 = p1.y;
		_x2 = p2.x;
		_y2 = p2.y;
		_color1 = color1;
		_color2 = color2;
		_uv1 = uv1;
		_uv2 = uv2;
	}
	else
	{
		_x1 = p2.x;
		_y1 = p2.y;
		_x2 = p1.x;
		_y2 = p1.y;
		_color1 = color2;
		_color2 = color1;
		_uv1 = uv2;
		_uv2 = uv1;
	}
}

WKGL::InnerLine::InnerLine(int startX, int endX, int y
	, float2 uvStart
	, float2 uvEnd
	, Rgba color1 /*= Rgba(255, 0, 0) */
	, Rgba color2 /*= Rgba(255, 0, 0) */) 
	: _y(y)
{
	if (endX >= startX)
	{
		_startX = startX;
		_endX = endX;
		_colorStart = color1;
		_colorEnd = color2;
		_uvStart = uvStart;
		_uvEnd = uvEnd;
	}
	else
	{
		_startX = endX;
		_endX = startX;
		_colorStart = color2;
		_colorEnd = color1;
		_uvStart = uvEnd;
		_uvEnd = uvStart;
	}
}
