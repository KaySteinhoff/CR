// THIS FILE ISN'T NEEDED TO RENDER MESHES

#include <crWindowing.h>

unsigned int shouldWindowClose = 0;

#if defined(__unix__)
	Display *display;
	Window window;
	Atom wmDeleteMessage;
	int defaultScreen;
#elif defined(__WIN32)
	#include <windows.h>
	MSG msg;
	HWND windowHandle;

	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
	{
		windowHandle = hwnd;
		switch(msg)
		{
			case WM_ERASEBKGND:
				return 1;
			case WM_CLOSE:
				shouldWindowClose = 1;
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			default:
				return DefWindowProc(hwnd, msg, wp, lp);
		}
		return 0;
	}

	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	{
		char *args[] = { "" };
		return main(1, args);
	}

#endif

unsigned int InitCRWindow(void)
{
#if defined(__unix__)
	display = XOpenDisplay(NULL);
	if(!display)
		return 0;

	wmDeleteMessage = XInternAtom(display, "WM_DELETE_MESSAGE", False);
	defaultScreen = DefaultScreen(display);

	window = XCreateSimpleWindow(display, RootWindow(display, defaultScreen), 0, 0, 800, 600, 0, BlackPixel(display, defaultScreen), WhitePixel(display, defaultScreen));

	if(!window)
	{
		XCloseDisplay(display);
		return 0;
	}

	XStoreName(display, window, "CR");
	XMapWindow(display, window);

	XSetWMProtocols(display, window, &wmDeleteMessage, 1);
#elif defined(__WIN32)
	HINSTANCE hInstance = GetModuleHandle(NULL);

	WNDCLASSEX wnd = {
		.cbSize = sizeof(WNDCLASSEX),
		.style = 0,
		.lpfnWndProc = WndProc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hInstance,
		.hIcon = LoadIcon(NULL, IDI_APPLICATION),
		.hCursor = LoadCursor(NULL, IDC_ARROW),
		.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
		.lpszMenuName = NULL,
		.lpszClassName = "CRWindow",
		.hIconSm = LoadIcon(NULL, IDI_APPLICATION),
	};

	if(!RegisterClassEx(&wnd))
	{
		MessageBox(NULL, "Failed to register 'CRWindow' class!", "RegisterWindowEx() error", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	HWND hwnd = CreateWindowEx(0, "CRWindow", "CR", WS_SYSMENU, 0, 0, 800, 600, NULL, NULL, hInstance, NULL);
	if(!hwnd)
	{
		MessageBox(NULL, "Failed to create 'CRWindow' instance!", "CreateWindowEx() error", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	windowHandle = hwnd;
	ShowWindow(hwnd, 5);
	UpdateWindow(hwnd);
#endif
	return 1;
}

unsigned int ShouldCRWindowClose(void)
{
#if defined(__unix__)
	if(XEventsQueued(display, QueuedAfterReading))
	{
		XEvent ev;
		XNextEvent(display, &ev);

		switch(ev.type)
		{
			case ClientMessage:
				if(ev.xclient.data.l[0] != wmDeleteMessage)
					break;
				shouldWindowClose = 1;
				break;
		}
	}
#elif defined(__WIN32)
	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#endif
	return shouldWindowClose;
}

void CloseCRWindow(void)
{
#if defined(__unix__)
	XMapWindow(display, 0);
	XDestroyWindow(display, window);
	XCloseDisplay(display);
#elif defined(__WIN32)
	DestroyWindow(windowHandle);
#endif
}
