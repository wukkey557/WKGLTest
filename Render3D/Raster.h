#ifndef _RASTER_H_
#define _RASTER_H_

#include <ctime>
#include "CELLMath.hpp"
#include "FreeImage.h"
#include "Image.h"

#pragma comment( lib, "FreeImage.lib" )

#define BUF_SIZE 800

#define SAFE_DEL(p) do { if (p) { delete (p); (p) = NULL; } } while (0);
 

namespace WKGL 
{
	typedef struct
	{
		float x;
		float y;
		float u;
		float v;
		Rgba color;
	} Vert;

	// 平行于x轴的内部线段
	class InnerLine 
	{
	public:
		InnerLine( int startX, int endX, int y
			, float2 uvStart
			, float2 uvEnd
			, Rgba color1 = Rgba(255, 0, 0)
			, Rgba color2 = Rgba(255, 0, 0) );
	public:
		int _startX; 
		int _endX; 
		int _y;
		Rgba _colorStart;
		Rgba _colorEnd;
		float2 _uvStart;
		float2 _uvEnd;
	};

	class Ege 
	{
	public:
		Ege( const int2& p1, const int2& p2
			, const float2& uv1, const float2& uv2
			, const Rgba& color1 = Rgba(255, 0, 0), const Rgba& color2 = Rgba(255, 0, 0) );

	public:
		int _x1;
		int _y1;
		float2 _uv1;
		int _x2;
		int _y2;
		float2 _uv2;
		Rgba _color1;
		Rgba _color2;
	};

	enum DATATYPE 
	{
		DT_BYTE = 0,
		DT_FLOAT,
		DT_DOUBLE
	};

	typedef struct 
	{
		int _size;	// 二维还是三维
		DATATYPE _type;
		int _stride; // 间隔,取完一个元素之后下一个元素偏移
		const void * _data;
	} DataElementDes;

	class Raster
	{
	public:
		enum DRAWMODE 
		{
			DM_POINTS = 0,
			DM_LINES,	// 线段,2点一条,4点2条线段
			DM_LINE_LOOP,	// 首尾相连,3个点3条线段
			DM_LINE_STRIP,	// 不首尾相连,3个点2条线段
			DM_TRIANGLE		// 三角形
		};

		// 三个点组成的结构体,包含顶点坐标,颜色,uv坐标
		struct Vertex 
		{
			int2 p0;
			int2 p1;
			int2 p2;
			Rgba c0;
			Rgba c1;
			Rgba c2;
			float2 uv0;
			float2 uv1;
			float2 uv2;
		};

		Raster();

		Raster(int width, int height, void * pBuffer, Rgba color = Rgba(255, 0, 0));
		~Raster();

		void clear();

		void drawImage(int x, int y, const Image * img);

		void vertexPointer( int size, DATATYPE type, int stride, const void * data ) 
		{
			_positionPointer._size = size;
			_positionPointer._type = type;
			_positionPointer._stride = stride;
			_positionPointer._data = data;
		}

		void colorPointer(int size, DATATYPE type, int stride, const void * data)
		{
			_colorPointer._size = size;
			_colorPointer._type = type;
			_colorPointer._stride = stride;
			_colorPointer._data = data;
		}

		void textureCoordPointer(int size, DATATYPE type, int stride, const void * data)
		{
			_uvPointer._size = size;
			_uvPointer._type = type;
			_uvPointer._stride = stride;
			_uvPointer._data = data;
		}

		void bindTexture( Image * image ) 
		{
			_image = image;
		}
		
		void drawTriangle( const Vertex& v )
		{
			if (!isInRect(v.p0) && !isInRect(v.p1) && !isInRect(v.p2))
			{
				return;
			}

			Ege eges[] =
			{
				Ege(v.p0, v.p1, v.uv0, v.uv1, v.c0, v.c1),
				Ege(v.p0, v.p2, v.uv0, v.uv2, v.c0, v.c2),
				Ege(v.p2, v.p1, v.uv2, v.uv1, v.c2, v.c1),
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

		void drawArrays( DRAWMODE mode, int startIdx, int count ) 
		{
			if ( _positionPointer._data == NULL )
				return;

			if ( _colorPointer._data == NULL )
				_colorPointer = _defColPointer;

			if (_uvPointer._data == NULL)
				_uvPointer = _defUVPointer;

			char * pVer = (char *)_positionPointer._data;
			char * pCol = (char *)_colorPointer._data;
			char * pUV = (char *)_uvPointer._data;



			for ( int i = startIdx; i < startIdx + count; i+= 3 )
			{
				float * pVer1 = (float *)pVer;
				float4 pp0 = float4(pVer1[0], pVer1[1], pVer1[1], 1);
				pVer += _positionPointer._stride;
				pVer1 = (float *)pVer;
				pp0 = _matModel * pp0;

				float4 pp1 = float4(pVer1[0], pVer1[1], pVer1[2], 1);
				pVer += _positionPointer._stride;
				pVer1 = (float *)pVer;
				pp1 = _matModel * pp1;

				float4 pp2 = float4(pVer1[0], pVer1[1], pVer1[2], 1);
				pVer += _positionPointer._stride;
				pVer1 = (float *)pVer;
				pp2 = _matModel * pp2;

				int2 p0 = int2(pp0.x, pp0.y);
				int2 p1 = int2(pp1.x, pp1.y);
				int2 p2 = int2(pp2.x, pp2.y);

				float * pUV1 = (float *)pUV;
				float2 uv0 = float2(pUV1[0], pUV1[1]);
				pUV += _uvPointer._stride;
				pUV1 = (float *)pUV;
				float2 uv1 = float2(pUV1[0], pUV1[1]);
				pUV += _uvPointer._stride;
				pUV1 = (float *)pUV;
				float2 uv2 = float2(pUV1[0], pUV1[1]);
				pUV += _uvPointer._stride;
				pUV1 = (float *)pUV;

				Rgba * pCol1 = (Rgba *)pCol;
				Rgba c0 = *pCol1;
				pCol += _colorPointer._stride;
				pCol1 = (Rgba *)pCol;
				Rgba c1 = *pCol1;
				pCol += _colorPointer._stride;
				pCol1 = (Rgba *)pCol;
				Rgba c2 = *pCol1;
				pCol += _colorPointer._stride;
				pCol1 = (Rgba *)pCol;

				Ege eges[] =
				{
					Ege(p0, p1, uv0, uv1, c0, c1),
					Ege(p0, p2, uv0, uv2, c0, c2),
					Ege(p2, p1, uv2, uv1, c2, c1),
				};

				drawTriangle(eges);

				// pVer += _positionPointer._stride;
				if ( !pCol )
					pCol = (char *)_colorPointer._data;

				if ( !pUV )
					pUV = (char *)_uvPointer._data;
			}
		}

		void drawTriangle( Ege eges[3] ) 
		{
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

		void loadMatrix( const matrix4& mat ) 
		{
			_matModel = mat;
		}

		void loadIdentify() 
		{
			_matModel = matrix4(1);
		}

	protected:
		// 根据2条边画一个第三边平行于x轴的三角形,e1为长边,e2为短边
		void drawEge1( const Ege& e1, const Ege& e2 );

		void drawEge(const Ege& e1, const Ege& e2)
		{
			float   yOffset1 = e1._y2 - e1._y1;
			if (yOffset1 == 0)
			{
				return;
			}

			float   yOffset = e2._y2 - e2._y1;
			if (yOffset == 0)
			{
				return;
			}
			float   xOffset = e2._x2 - e2._x1;
			float   scale = 0;
			float   step = 1.0f / yOffset;


			float   xOffset1 = e1._x2 - e1._x1;
			float   scale1 = (float)(e2._y1 - e1._y1) / yOffset1;
			float   step1 = 1.0f / yOffset1;

			for (int y = e2._y1; y < e2._y2; ++y)
			{
				int     x1 = e1._x1 + (int)(scale1 * xOffset1);
				int     x2 = e2._x1 + (int)(scale * xOffset);
				Rgba    color2 = colorLerp(e2._color1, e2._color2, scale);
				Rgba    color1 = colorLerp(e1._color1, e1._color2, scale1);

				float2  uvStart = uvLerp(e1._uv1, e1._uv2, scale1);
				float2  uvEnd = uvLerp(e2._uv1, e2._uv2, scale);

				InnerLine    span(x1, x2, y, uvStart, uvEnd, color1, color2);
				drawInnerLine(span);

				scale += step;
				scale1 += step1;
			}
		}
	protected:

		bool isInRect(const int2& p);

		void drawInnerLine( const InnerLine& line );

		void drawPoint( float2 pos, Rgba color );
		void drawPoint(int posX, int posY, Rgba color, unsigned ptSize);

		void drawLine(float2 posSrc, float2 posDst, Rgba color1, Rgba color2);

		// 斜率
		float getSlope(float2 src, float2 dst);

		// cotan斜率
		float getCotSlope(float2 src, float2 dst);

		inline Rgba getPixel( uint x, uint y ) 
		{
			return Rgba( _buf[y * _width + x] );
		}

		inline void setPixel(unsigned x, unsigned y, Rgba color)
		{
			if (x >= _width || y >= _height)
			{
				return;
			}

			//_buf[y][x] = color;
			*(_buf + _width * y + x) = color;
		}

		inline void setPixelEx(unsigned x, unsigned y, Rgba color)
		{
			*(_buf + _width * y + x) = color;
		}
	
	public:
		Rgba * _buf;
		int _width;
		int _height;
		Rgba _color;

		DataElementDes _positionPointer;
		DataElementDes _uvPointer;
		DataElementDes _colorPointer;

		DataElementDes _defColPointer;
		DataElementDes _defUVPointer;
		Rgba _defColArr[3];
		float2 _defUVArr[3];
	protected:
		Image * _image;
		matrix4 _matModel;
	};
}
#endif