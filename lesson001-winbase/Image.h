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

	public:
		Image( int w, int h, void * buf = NULL );

		~Image();

		static Image * loadFromFile( const char * fileName );

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
	};
}

#endif