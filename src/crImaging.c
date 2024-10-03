// THIS FILE ISN'T NEEDED TO RENDER MESHES

#if defined(__unix__)
	#include <X11/Xlib.h>
	#include <stdio.h>
#endif
#include <crWindowing.h>
#include <crImaging.h>
#include <stdlib.h>

CRImage CreateRenderImage(unsigned int width, unsigned int height)
{
	CRImage image = {
		.width = width,
		.height = height
	};
#if defined(__unix__)

	image.data = malloc((width*height)<<2);
	if(!image.data)
		return (CRImage) { 0 };

	image.xImg = XCreateImage(display, DefaultVisual(display, defaultScreen), 24, ZPixmap, 0, (char*)image.data, width, height, 32, width<<2);
	if(!image.xImg)
	{
		printf("XCreateImage failed!\n");
		free(image.data);
		return (CRImage) { 0 };
	}
#elif defined(__WIN32)
	HDC hdc = GetDC(windowHandle);

	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	image.hBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&image.data, NULL, 0);

	ReleaseDC(windowHandle, hdc);
#endif
	return image;
}

void DrawRenderImage(CRImage image, unsigned int x, unsigned int y)
{
#if defined(__unix__)
	XPutImage(display, window, DefaultGC(display, defaultScreen), image.xImg, 0, 0, x, y, image.width, image.height);
#elif defined(__WIN32)
	PAINTSTRUCT ps;
	RECT r = { .top = y, .left = x, .bottom = y+image.height, .right = x+image.width};
	InvalidateRect(windowHandle, &r, TRUE);
	HDC hdc = BeginPaint(windowHandle, &ps);

	HDC hdcMem = CreateCompatibleDC(hdc);

	HGDIOBJ gdiBmp = SelectObject(hdcMem, image.hBmp);
	BitBlt(hdc, x, y, image.width, image.height, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, gdiBmp);

	DeleteDC(hdcMem);

	EndPaint(windowHandle, &ps);
	ReleaseDC(windowHandle, hdc);
#endif
}

void freeRenderImage(CRImage image)
{
#if defined(__unix__)
	// idk why but I can't use XDestroyImage
	XFree(image.xImg->data);
	XFree(image.xImg->obdata);
	XFree(image.xImg);
#elif defined(__WIN32)
	DeleteObject(image.hBmp);
#endif
	image.data = NULL;
}
