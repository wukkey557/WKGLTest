#include <windows.h>
#include "Raster.h"

using namespace WKGL;

#ifdef _WIN32 
	#ifdef _DEBUG
		#define WK_WIN32_DEBUG
	#else
		#define WK_WIN32_RELEASE
	#endif 
#endif

int RedirectToConsole()
{
#ifdef WK_WIN32_DEBUG
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
#endif
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SIZE:
	break;
	case WM_CLOSE:
	case WM_DESTROY:
	PostQuitMessage(0);
	break;
	default:
	break;
	}

	return  DefWindowProc(hWnd, msg, wParam, lParam);
}

void  getResourcePath(HINSTANCE hInstance, char pPath[1024])
{
	char    szPathName[1024];
	char    szDriver[64];
	char    szPath[1024];
	// 获得exe所在路径
	GetModuleFileNameA(hInstance, szPathName, sizeof(szPathName));
	_splitpath(szPathName, szDriver, szPath, 0, 0);
	sprintf(pPath, "%s%s", szDriver, szPath);
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
	) 
{
	if (RedirectToConsole() != 0)
	{
		printf("RedirectToConsole fialed!\n");
	}

	WNDCLASSEX wndClass;
	memset( &wndClass, 0, sizeof(wndClass) );
	wndClass.lpszClassName = L"WkWindow";
	wndClass.cbSize = sizeof(wndClass);
	wndClass.style  = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = 0;
	wndClass.hIconSm = 0;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	RegisterClassEx(&wndClass);

	//  2 创建窗口
	HWND    hWnd = CreateWindowEx(
		NULL,
		L"WkWindow",
		L"wk",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		0,
		0,
		BUF_SIZE,
		BUF_SIZE,
		0,
		0,
		hInstance,
		0
		);

	UpdateWindow(hWnd);
	ShowWindow(hWnd, SW_SHOW);

	// get window rect
	RECT rect = {0};
	GetClientRect(hWnd, &rect);

	int width = rect.right - rect.left;
	int height = abs( rect.bottom - rect.top );
	void * pBuffer = 0;
	// GetDC取得与窗口客户区相关的dc，GetWindowDC取得与整个窗口（包括客户区和非客户区）相关的dc
	HDC hDC = GetDC(hWnd);
	// 创建一块兼容dc
	HDC hMem = CreateCompatibleDC( hDC );

	BITMAPINFO	bmpInfor;
	bmpInfor.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	/*bmpInfor.bmiHeader.biWidth = width;
	bmpInfor.bmiHeader.biHeight = height;*/

	bmpInfor.bmiHeader.biWidth = BUF_SIZE;
	bmpInfor.bmiHeader.biHeight = -BUF_SIZE;

	bmpInfor.bmiHeader.biPlanes = 1;
	bmpInfor.bmiHeader.biBitCount = 32;
	bmpInfor.bmiHeader.biCompression = BI_RGB;
	bmpInfor.bmiHeader.biSizeImage = 0;
	bmpInfor.bmiHeader.biXPelsPerMeter = 0;
	bmpInfor.bmiHeader.biYPelsPerMeter = 0;
	bmpInfor.bmiHeader.biClrUsed = 0;
	bmpInfor.bmiHeader.biClrImportant = 0;
	// 根据位图结构信息分配内存空间
	HBITMAP hBm = CreateDIBSection( hDC, &bmpInfor, DIB_RGB_COLORS, &pBuffer, 0, 0 );
	SelectObject( hMem, hBm );

	// Raster rst(BUF_SIZE, BUF_SIZE, pBuffer);

	char    szPath[1024];
	getResourcePath(0, szPath);

	char szImage[1024];
	sprintf(szImage, "%s/image/bg.png", szPath);
	Image * image = Image::loadFromFile(szImage);

	sprintf( szImage, "%s/image/grass.png", szPath );
	Image * image1 = Image::loadFromFile(szImage);

	Raster rst(BUF_SIZE, BUF_SIZE, pBuffer);

	MSG msg = {0};
	while (true) 
	{
		if (msg.message == WM_DESTROY
			|| msg.message == WM_CLOSE
			|| msg.message == WM_QUIT)
		{
			break;
		}
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//memset( pBuffer, 0, 4 * width * height );
		//unsigned char * pBuf = (unsigned char *)pBuffer;
		//memset( pBuf + 10 * 4 * width, 255, 10 * width * 4 );
		//// 用于从原设备中复制位图到目标设备
		//BitBlt( hDC, 0, 0, width, height, hMem, 0, 0, SRCCOPY );

		rst.clear();
		/*for ( int i = 0; i < 100; i++ )
		{
		rst.drawPoint( rand() % rst._width, rand() % rst._height, Rgba(255, 0, 0)
		, 2 );
		}*/

		
		/*const float2 pArr[4] = { float2(100, 100), float2(200, 200)
			, float2(200, 100), float2(50, 50) };
		rst.drawArrays( Raster::DM_LINES, pArr, 4);*/

		/*const float2 pArr[3] = { float2(100, 100), float2(200, 200)
			, float2(200, 50) };
			rst.drawArrays(Raster::DM_LINE_LOOP, pArr, 3);*/

		/*const float2 pArr[3] = { float2(100, 100), float2(200, 200)
			, float2(200, 50) };
		rst.drawArrays(Raster::DM_LINE_STRIP, pArr, 3);*/

		/*const float2 pArr[3] = { float2(100, 100), float2(200, 200)
			, float2(200, 50) };
		rst.drawArrays(Raster::DM_POINTS, pArr, 3);*/

		//// bezier
		//// 贝塞尔曲线4个点确定 : 起点,终点,中间俩点确定曲度
		//const float2 points[] = 
		//{
		//	float2(0, 0),
		//	float2(0, 100),
		//	float2(50, 100),
		//	float2(100, 0)
		//};
		//// 根据贝塞尔曲线3次方公式
		//// 0.01表示粒度,将曲线分为100个线段来绘制
		//float2 prev[2];
		//for ( float t = 0; t < 1.0f; t += 0.01f )
		//{
		//	float x = points[0].x * pow(1 - t, 3)
		//		+ 3 * points[1].x * t * pow(1 - t, 2)
		//		+ 3 * points[2].x * t*t * (1 - t)
		//		+ points[3].x * t * t * t;
		//	float y = points[0].y * pow(1 - t, 3)
		//		+ 3 * points[1].y * t * pow(1 - t, 2)
		//		+ 3 * points[2].y * t*t * (1 - t)
		//		+ points[3].y * t * t * t;

		//	if ( t != 0 )
		//	{
		//		prev[1] = float2(x, y);
		//		rst.drawArrays( Raster::DM_LINES, prev, 2 );
		//	}
		//	else 
		//	{
		//		prev[1] = float2(x, y);
		//	}
		//	prev[0] = prev[1];
		//}

		// rst.drawFilledRect( 10, 50, 300, 200 );

		/*const int2 points[] = 
		{
			int2(10, 10),
			int2(100, 10),
			int2(100, 100),
			int2(10, 100)
		};

		const Rgba colors[] = 
		{
			Rgba(255, 0, 0),
			Rgba(0, 255, 0),
			Rgba(0, 0, 255),
			Rgba(0, 0, 0),
		};

		rst.drawRect( points, colors );*/

		/*const Ege e1( int2(100, 50), int2(50, 100) );
		const Ege e2( int2(100, 50), int2(150, 100) );
		rst.drawEge( e1, e2 );*/

		/*int2  pt[3] =
		{
			int2(150, 10),
			int2(-50, 60),
			int2(280, 200),
		};


		const Rgba colors[] =
		{
			Rgba(255, 0, 0),
			Rgba(0, 255, 0),
			Rgba(0, 0, 255),
		};

		rst.drawTriangle(pt, colors);*/

		// rst.drawImage( 50, 50, 150, 150 );

		// rst.drawImage(100, 100, image);

		// rst.drawImageWithColorKey( 10, 10, image, Rgba(255, 0, 0) );

		rst.drawImage(10, 10, image);
		// rst.drawImageAlphaTest( 50, 50, image1, 100 );

		// rst.drawImageAlphaBlend( 50, 50, image1, 0.2 );
		// rst.drawImageAlpha( 50, 50, image1, 0.5f );

		// rst.drawImage(20, 50, image1, 30, 30, 50, 50);

		// 图片缩放
		rst.drawImageScale(20, 20, 256, 256, image1);

		memcpy( pBuffer, rst._buf, rst._width * rst._height * sizeof(Rgba) );

		

		BitBlt(hDC, 0, 0, width, height, hMem, 0, 0, SRCCOPY);
	}

	SAFE_DEL(image);
	SAFE_DEL(image1);

	#ifdef WK_WIN32_DEBUG
		FreeConsole();
	#endif

	return 0;
}