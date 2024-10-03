// THIS FILE ISN'T NEEDED TO RENDER MESHES

#ifndef CR_WINDOWING_H_
#define CR_WINDOWING_H_

#if defined(__unix__)
	#include <X11/Xlib.h>
	extern Display *display;
	extern Window window;
	extern int defaultScreen;
#elif defined(__WIN32)
	#include <windows.h>

	extern HWND windowHandle;
	extern int main(int argc, char **argv);
#else
	#error "Platform not supported"
#endif

unsigned int InitCRWindow(void);
unsigned int ShouldCRWindowClose(void);
void CloseCRWindow(void);

#endif
