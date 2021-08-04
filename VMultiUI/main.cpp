#include "pch.h"

class App
{
public:
	static App& Instance();
	std::map<const char *, HWND> WindowHandles;
	HINSTANCE hInstance{ nullptr };
	WindowsXamlManager WindowManager { nullptr };
	DesktopWindowXamlSource DesktopSource { nullptr };
	HMENU Menu;
	HACCEL KeyboardTable;
	void CreateMainWindow();
	void AttachUserControl();
	const wchar_t* WindowClass = L"Win32DesktopApp"; // Window class for main window
	bool QuitApp = false;
	static void OnCommand(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // Process WM_COMMAND messages
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // Process Windows messages
private:
	App() { WindowHandles = { {"Program", NULL}, {"MainWindow", NULL}, {"ChildWindow", NULL}, {"Control", NULL}}; };
	App(const App&) {};
	App& operator=(const App&) {};
	~App() {};
};

App& App::Instance()
{
	static App instance;
	return instance;
}

void App::OnCommand(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool isMenu = (wParam >> 16 == 1);
	if ((wParam & 0xFFFF) == ID_ACCEL_EXIT || (wParam & 0xFFFF) == ID_FILE_EXIT) { App::Instance().QuitApp = true; }
	else if ((wParam & 0xFFFF) == ID_ACCEL_ABOUT || (wParam & 0xFFFF) == ID_HELP_ABOUT) {
		std::cout << "About: This is a minimal WinRT program, v1.0." << std::endl;
	}
}

LRESULT App::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		case WM_CREATE: {
			CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
			if (cs->hwndParent == App::Instance().WindowHandles.at("Program")) {
				App::Instance().WindowHandles.at("MainWindow") = hwnd;
				App::Instance().WindowHandles.at("ChildWindow") = CreateWindowEx(NULL, App::Instance().WindowClass, L"", WS_CHILD | WS_VISIBLE, 0, 0, 100, 100, App::Instance().WindowHandles.at("MainWindow"), NULL, App::Instance().hInstance, NULL);
				App::Instance().AttachUserControl();
				ShowWindow(App::Instance().WindowHandles.at("ChildWindow"), 1);
				UpdateWindow(App::Instance().WindowHandles.at("ChildWindow"));
				SetFocus(App::Instance().WindowHandles.at("ChildWindow"));
			}
		} break;
		case WM_SIZE: {
			int width = LOWORD(lParam), height = HIWORD(lParam);
			RECT rect; GetClientRect(hwnd, &rect);
			if (App::Instance().WindowHandles.at("Control") != NULL) {
				SetWindowPos(App::Instance().WindowHandles.at("ChildWindow"), 0, 0, 0, width, height, SWP_SHOWWINDOW);
				SetWindowPos(App::Instance().WindowHandles.at("Control"), 0, 0, 0, width, height, SWP_SHOWWINDOW);
			}
			InvalidateRect(hwnd, &rect, true);
		} break;
		case WM_PAINT: {
			PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			EndPaint(hwnd, &ps);
		} break;
		case WM_COMMAND: { OnCommand(hwnd, uMsg, wParam, lParam); } break;
		case WM_CLOSE: { App::Instance().QuitApp = true; } break;
		default: break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void App::CreateMainWindow()
{
	WNDCLASSEX windowClass = { }; 
	windowClass.cbSize = sizeof(WNDCLASSEX); 
	windowClass.lpfnWndProc = App::WindowProc;
	windowClass.hInstance = App::Instance().hInstance;
	windowClass.lpszClassName = App::Instance().WindowClass;
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	RegisterClassEx(&windowClass);
	App::Instance().Menu = LoadMenu(App::Instance().hInstance, (LPCWSTR)MAKEINTRESOURCE(IDR_MENU1));
	App::Instance().KeyboardTable = LoadAccelerators(App::Instance().hInstance, (LPCWSTR)MAKEINTRESOURCE(IDR_ACCELERATOR1));
	App::Instance().WindowHandles.at("MainWindow") = CreateWindow(App::Instance().WindowClass, L"This is a minimal console C++ app using WinUI3", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 700, 350, App::Instance().WindowHandles.at("Program"), App::Instance().Menu, App::Instance().hInstance, NULL);
	ShowWindow(App::Instance().WindowHandles.at("MainWindow"), 1); 
	UpdateWindow(App::Instance().WindowHandles.at("MainWindow"));
}

void App::AttachUserControl()
{
	VMultiUWPApp_WRC::MyUserControl ctl = VMultiUWPApp_WRC::MyUserControl();
	ctl.as<FrameworkElement>().FindName(L"accelAbout").as<KeyboardAccelerator>().Invoked([&](IInspectable const& sender, KeyboardAcceleratorInvokedEventArgs const& args) {
		App::Instance().OnCommand(App::Instance().WindowHandles.at("ChildWindow"), WM_COMMAND, ID_ACCEL_ABOUT, 0); args.Handled(true);
	});
	ctl.as<FrameworkElement>().FindName(L"accelExit").as<KeyboardAccelerator>().Invoked([&](IInspectable const& sender, KeyboardAcceleratorInvokedEventArgs const& args) {
		App::Instance().OnCommand(App::Instance().WindowHandles.at("ChildWindow"), WM_COMMAND, ID_ACCEL_EXIT, 0); args.Handled(true);
	});
	App::Instance().DesktopSource.Content(ctl);
	auto interop = App::Instance().DesktopSource.as<IDesktopWindowXamlSourceNative>();
	HRESULT attachResult = interop->AttachToWindow(App::Instance().WindowHandles.at("ChildWindow"));
	HRESULT getResult = interop->get_WindowHandle(&App::Instance().WindowHandles.at("Control"));
}

int main(int argc, char **argv)
{
	std::wcout.imbue(std::locale(".UTF8"));
	App::Instance().hInstance = GetModuleHandle(NULL);
	App::Instance().WindowHandles.at("Program") = GetConsoleWindow();
	winrt::init_apartment(winrt::apartment_type::single_threaded);
	App::Instance().WindowManager = WindowsXamlManager::InitializeForCurrentThread();
	App::Instance().DesktopSource = DesktopWindowXamlSource();
	App::Instance().CreateMainWindow();
	MSG msg; // Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0) && !App::Instance().QuitApp) {
		auto src = App::Instance().DesktopSource.as<IDesktopWindowXamlSourceNative2>();
		BOOL preTranslated = false; src->PreTranslateMessage(&msg, &preTranslated);
		if (!preTranslated)
		{
			int translate = TranslateAccelerator(msg.hwnd, App::Instance().KeyboardTable, &msg);
			if (!translate) { TranslateMessage(&msg); DispatchMessage(&msg); }
			else {
				if (msg.hwnd != App::Instance().WindowHandles.at("Program") && msg.hwnd != App::Instance().WindowHandles.at("MainWindow") && msg.hwnd != App::Instance().WindowHandles.at("ChildWindow") && msg.hwnd != App::Instance().WindowHandles.at("Control")) {
					// Hwnd is likely a Windows.UI.Input.InputSite.WindowClass, so we will pass through KeyboardAccelerator keys
					TranslateMessage(&msg); DispatchMessage(&msg);
				}
			}
		}
	}
	return (int)msg.wParam;
}