#include <windows.h>

struct My_Window {
  uint8_t blob[24];
};

struct Win32_Window {
  uint8_t blob[8];
};
//static_assert(sizeof(Win32_Window) <= sizeof(My_Window));

void create_window(My_Window* r) {
  Win32_Window w;
  memset(&w, 0x01, sizeof(Win32_Window));

  *r = {};
  *(Win32_Window*)r = w;
}

void destroy_window(My_Window* r) {
  Win32_Window* w = (Win32_Window*)r;

  // here we can use specific Win32_Window.
  *w = {};
}

#if 0
LRESULT CALLBACK main_window_callback(HWND handle, UINT message, WPARAM w_param, LPARAM l_param) {
  return DefWindowProc(handle, message, w_param, l_param);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	WNDCLASSEX window_class_ex = {};
	window_class_ex.cbSize 			= sizeof(WNDCLASSEX);
	window_class_ex.style       = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	window_class_ex.lpfnWndProc = main_window_callback;
  window_class_ex.hInstance   = hInstance;
  window_class_ex.lpszClassName = "Non-Blocking UI";

	if(RegisterClassEx(&window_class_ex) == 0) {
    // report error.
    assert(0); return 1;
	}

	HWND window_handle = CreateWindowEx(
                            0,
                            window_class_ex.lpszClassName,
                            "UI",                             // window title
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE, // window style
                            CW_USEDEFAULT,                    // x
                            CW_USEDEFAULT,                    // y
                            CW_USEDEFAULT,                    // width
                            CW_USEDEFAULT,                    // height 
                            0,                                // parent handle
                            0,                                // menu   handle
                            hInstance,                        // window instance
                            0);                               // l_param on WM_CREATE message in wnd_proc
  if(!window_handle) {
    // report error.
    assert(0); return 1;
  }

  while(1) {
    MSG message;
    BOOL result = GetMessage(&message, 0, 0, 0);
    if(result <= 0) break;

    TranslateMessage(&message);
    DispatchMessage(&message);
  }

  return 0;
}
#endif
