//#include <windows.h>
#include <cr.h>
#include <crWindowing.h>
#include <crImaging.h>
#include <time.h>
#include <string.h>
//#include <sys/time.h>
#include <stdio.h>

static const int screenWidth = 800;
static const int screenHeight = 600;

vertex model[] = {
	/* FRONT FACE */
	{ .position = { .x = 0, .y = 0, .z = 0} },
	{ .position = { .x = 1, .y = 0, .z = 0} },
	{ .position = { .x = 1, .y = 1, .z = 0} },

	{ .position = { .x = 0, .y = 0, .z = 0} },
	{ .position = { .x = 0, .y = 1, .z = 0} },
	{ .position = { .x = 1, .y = 1, .z = 0} },
	/* LEFT FACE */
	{ .position = { .x = 0, .y = 0, .z = 0} },
	{ .position = { .x = 0, .y = 0, .z = 1} },
	{ .position = { .x = 0, .y = 1, .z = 1} },

	{ .position = { .x = 0, .y = 0, .z = 0} },
	{ .position = { .x = 0, .y = 1, .z = 0} },
	{ .position = { .x = 0, .y = 1, .z = 1} },
	/* RIGHT FACE */
	{ .position = { .x = 1, .y = 0, .z = 0} },
	{ .position = { .x = 1, .y = 0, .z = 1} },
	{ .position = { .x = 1, .y = 1, .z = 1} },

	{ .position = { .x = 1, .y = 0, .z = 0} },
	{ .position = { .x = 1, .y = 1, .z = 0} },
	{ .position = { .x = 1, .y = 1, .z = 1} },
	/* BOTTOM FACE */
	{ .position = { .x = 0, .y = 0, .z = 0} },
	{ .position = { .x = 1, .y = 0, .z = 0} },
	{ .position = { .x = 1, .y = 0, .z = 1} },

	{ .position = { .x = 0, .y = 0, .z = 0} },
	{ .position = { .x = 0, .y = 0, .z = 1} },
	{ .position = { .x = 1, .y = 0, .z = 1} },
	/* TOP FACE */
	{ .position = { .x = 0, .y = 1, .z = 0} },
	{ .position = { .x = 1, .y = 1, .z = 0} },
	{ .position = { .x = 1, .y = 1, .z = 1} },

	{ .position = { .x = 0, .y = 1, .z = 0} },
	{ .position = { .x = 0, .y = 1, .z = 1} },
	{ .position = { .x = 1, .y = 1, .z = 1} },
	/* BACK FACE */
	{ .position = { .x = 0, .y = 0, .z = 1} },
	{ .position = { .x = 1, .y = 0, .z = 1} },
	{ .position = { .x = 1, .y = 1, .z = 1} },

	{ .position = { .x = 0, .y = 0, .z = 1} },
	{ .position = { .x = 0, .y = 1, .z = 1} },
	{ .position = { .x = 1, .y = 1, .z = 1} },
};

vertex cubeTriStrip[] = {
	{ .position = { .x = 0, .y = 0, .z = 0} },
	{ .position = { .x = 1, .y = 0, .z = 0} },
	{ .position = { .x = 0, .y = 0, .z = 1} },
	{ .position = { .x = 1, .y = 0, .z = 1} },
	{ .position = { .x = 0, .y = 1, .z = 1} },
	{ .position = { .x = 1, .y = 1, .z = 1} },
	{ .position = { .x = 0, .y = 1, .z = 0} },
	{ .position = { .x = 1, .y = 1, .z = 0} },
	{ .position = { .x = 0, .y = 0, .z = 0} },
	{ .position = { .x = 1, .y = 0, .z = 0} }
};

/*typedef struct
{
	HBITMAP hBmp;
	unsigned char *data;
}Buffer;

RECT screenRect;
Buffer render;*/
clock_t start, end;

/*RECT r = {
	.top = 0,
	.left = 0,
	.bottom = 1,
	.right = 1
};
char renderTimeTxt[32] = { 0 };
float renderTime = 0;
unsigned int renderTimes[32] = { 0 };
int currentRenderTime = 0;
struct timeval start, end;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);*/

int main(int argc, char **argv)
{
	if(!InitCRWindow())
		return 0;

	if(!InitCR(screenWidth, screenHeight, 90, 0.1, 1000))
	{
#if defined(__WIN32)
		MessageBox(NULL, "Failed to initialize cRender lib!", "InitCR() error", MB_OK | MB_ICONEXCLAMATION);
#elif defined(__unix__)
		printf("Failed to initialize cRender lib!\n");
#endif
		return 1;
	}

	CRImage image = CreateRenderImage(screenWidth, screenHeight);
	if(!image.data)
	{
		printf("Failed to create render image!\n");
		return 2;
	}
	SetRenderDestination(image.data);
	start = clock();
	crTransform transform = {
		.position = { .x = 0, .y = 0, .z = 3},
		.rotation = { 0 },
		.scale = { .x = 1, .y = 1, .z = 1 }
	};

	while(!ShouldCRWindowClose())
	{
		memset(image.data, 0, (screenWidth * screenHeight)<<2);
		end = clock();
		if(end-start < CLOCKS_PER_SEC * (1.0/30.0)) continue;
		float elapsed = (float)(end-start)/CLOCKS_PER_SEC;
		transform.rotation.x += elapsed*0.5*50;
		transform.rotation.z += elapsed*50;
		RenderModel(model, 36, transform);
		start = end;
		DrawRenderImage(image, 0, 0);
	}

	freeRenderImage(image);
	CloseCRWindow();

	return 0;
}

/*LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, hdcMem;
	PAINTSTRUCT ps;
	HGDIOBJ gdiBmp;
	
	switch(msg)
	{
		case WM_ERASEBKGND:
			return 1;
		case WM_TIMER:
			gettimeofday(&start, NULL);
			//RenderModel(model, 36, (float)(clock()-timer)/CLOCKS_PER_SEC);
			RenderTriangleStrip(cubeTriStrip, sizeof(cubeTriStrip)/sizeof(cubeTriStrip[0]), (float)(clock()-timer)/CLOCKS_PER_SEC);
			timer = clock();
			gettimeofday(&end, NULL);
			
			renderTimes[currentRenderTime++] = end.tv_usec-start.tv_usec;
			currentRenderTime &= 31;
			
			InvalidateRect(hwnd, &screenRect, TRUE);
			break;
		case WM_CREATE:
			hdc = GetDC(hwnd);
			
			BITMAPINFO bmi = { 0 };
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = screenWidth;
			bmi.bmiHeader.biHeight = -screenHeight;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			
			render.hBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&render.data, NULL, 0);
			SetRenderDestination(render.data);
			SetTimescale(1);

			ReleaseDC(hwnd, hdc);
			timer = clock();
			DrawText(hdc, renderTimeTxt, 31, &r, DT_CALCRECT);
			break;
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			
			hdcMem = CreateCompatibleDC(hdc);

			gdiBmp = SelectObject(hdcMem, render.hBmp);
			BitBlt(hdc, 0, 0, screenWidth, screenHeight, hdcMem, 0, 0, SRCCOPY);
			SelectObject(hdcMem, gdiBmp);

			DeleteDC(hdcMem);
			
			if(currentRenderTime == 31)
			{
				memset(renderTimeTxt, 0, 31);
				unsigned int avgTime = renderTimes[0];
				for(int i = 1; i < 32; ++i)
					avgTime += renderTimes[i];
				snprintf(renderTimeTxt, 31, "%d", avgTime>>5);
			}
			DrawText(hdc, renderTimeTxt, 31, &r, DT_LEFT | DT_TOP | DT_NOCLIP);
			EndPaint(hwnd, &ps);
			ReleaseDC(hwnd, hdc);
			SetTimer(hwnd, 0, 0, NULL);
			break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	
	return 0;
}
*/
