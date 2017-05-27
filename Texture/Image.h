#ifndef _WKGL_IMAGE_H_
#define _WKGL_IMAGE_H_

#include "CELLMath.hpp"
#include "FreeImage.h"

namespace WKGL 
{
	class Image
	{
	protected:
		int _width;
		int _height;
		uint * _pixel;
		int _wrapType;	// 最后点颜色填充 : 1, 纹理重复 : 0
	public:
		Image( int w, int h, void * buf = NULL );

		~Image();

		static Image * loadFromFile( const char * fileName );

		void setWrapType(int type);

		inline int getWidth() const
		{
			return _width;
		}

		inline int getHeight() const
		{
			return _height;
		}

		inline Rgba pixelAt(int x, int y) const
		{
			// uint ss = _pixel[y * _width + x];
			// printf("uuu==>>%x\n", ss);
			return Rgba( _pixel[y * _width + x] );
		}

		inline Rgba pixelUVAt(const tvec2<float>& uv) const 
		{
			float x = _width * uv.x;
			float y = _height * uv.y;

			if (_wrapType == 0)
				return pixelAt( (unsigned)x % _width, (unsigned)y % _height);
			else 
			{
				if (x >= _width)
				{
					x = _width - 1;
				}
				if (y >= _height)
				{
					y = _height - 1;
				}
				return pixelAt(x, y);
			}
		}
	};
}

#endif