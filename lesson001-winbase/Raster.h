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
	// 平行于x轴的内部线段
	class InnerLine 
	{
	public:
		InnerLine( int startX, int endX, int y
			, Rgba color1 = Rgba(255, 0, 0)
			, Rgba color2 = Rgba(255, 0, 0) );
	public:
		int _startX; 
		int _endX; 
		int _y;
		Rgba _colorStart;
		Rgba _colorEnd;
	};

	class Ege 
	{
	public:
		Ege( const int2& p1, const int2& p2
			, const Rgba& color1 = Rgba(255, 0, 0), const Rgba& color2 = Rgba(255, 0, 0) );

	public:
		int _x1;
		int _y1;
		int _x2;
		int _y2;
		Rgba _color1;
		Rgba _color2;
	};

	class Raster
	{
	public:
		enum DRAWMODE 
		{
			DM_POINTS = 0,
			DM_LINES,	// 线段,2点一条,4点2条线段
			DM_LINE_LOOP,	// 首尾相连,3个点3条线段
			DM_LINE_STRIP	// 不首尾相连,3个点2条线段
		};

		Raster();

		Raster(int width, int height, void * pBuffer, Rgba color = Rgba(255, 0, 0));
		~Raster();

		void clear();
		void drawArrays( DRAWMODE mode, const float2 * pArr, int num );

		// 单色填充
		void drawFilledRect( int x, int y, int w, int h, Rgba color = Rgba(255, 0, 0) );

		// 渐变差值填充
		void drawRect( const int2 * points, const Rgba * colors );

		void drawTriangle( const int2 * points, const Rgba * colors, int count = 3 );

		void drawImage( int x, int y, int w, int h );

		void drawImage(int x, int y, const Image * img);

		// key : 想要屏蔽的颜色
		void drawImageWithColorKey(int x, int y, const Image * img, const Rgba& key);

		void drawImageAlphaTest(int x, int y, const Image * img, BYTE alpha);

		void drawImageAlphaBlend(int x, int y, const Image * img, float alpha = 1.0f);

		void drawImageAlpha(int x, int y, const Image * img, float alpha = 1.0f);

		void drawImage(int startX, int startY, const Image * img, int x, int y
			, int w, int h);

		// 图片缩放(最近点采样)
		void drawImageScale(int destX, int destY, int destW, int destH, const Image * img);

	protected:
		// 根据2条边画一个第三边平行于x轴的三角形,e1为长边,e2为短边
		void drawEge( const Ege& e1, const Ege& e2 );


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
	};
}
#endif