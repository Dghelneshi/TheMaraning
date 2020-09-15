#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define NODRAWTEXT
#define NOGDI
#define NOMEMMGR
#define NOSERVICE
#define NOSOUND
#define NOWH
#define NOHELP
#define NOMCX
#define NOATOM
//#define NOCOLOR
#include <windows.h>
#include <shellapi.h>
#include <locale.h>

#include "main/common.h"
#include "main/main.h"
#include "platform/platform.h"

// this needs to be included in one file on windows
#include "thirdparty/rpmalloc/rpnew.h"

[[nodiscard]] static std::unique_ptr<char[]> WideCharToUTF8(const wchar_t* wide_string) {
	int len = WideCharToMultiByte(CP_UTF8, 0, wide_string, -1, nullptr, 0, nullptr, nullptr);
	std::unique_ptr<char[]> buf(new char[len + 1]);
	WideCharToMultiByte(CP_UTF8, 0, wide_string, -1, buf.get(), len, nullptr, nullptr);
	buf[len] = '\0';
	return buf;
}

[[nodiscard]] static std::unique_ptr<wchar_t[]> UTF8ToWideChar(const char* string) {
	int len = MultiByteToWideChar(CP_UTF8, 0, string, -1, nullptr, 0);
	std::unique_ptr<wchar_t[]> buf(new wchar_t[len + 1]);
	MultiByteToWideChar(CP_UTF8, 0, string, -1, buf.get(), len);
	buf[len] = '\0';
	return buf;
}

static HINSTANCE G_hInstance;
static HWND G_hWnd;

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		//ValidateRect(hWnd, nullptr);
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_DESTROY:
	{
		Main::Exit();
		PostQuitMessage(0);
		return 0;
	}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


// Public Interface
namespace Platform
{
	void DebugOutputA(const char* string)
	{
		OutputDebugStringA(string);
	}
	void DebugOutput(const char* string)
	{
		auto wstr = UTF8ToWideChar(string);
		OutputDebugStringW(wstr.get());
	}
	void DebugOutputPrintfA(const char* format, ...)
	{
		va_list args;
		char buffer[512];

		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		OutputDebugStringA(buffer);
	}
	void DebugOutputPrintf(const char* format, ...)
	{
		va_list args;
		char buffer[512];

		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		auto wstr = UTF8ToWideChar(buffer);
		OutputDebugStringW(wstr.get());
	}

	void CreateMainWindow(WindowMode mode)
	{
		WNDCLASSEX windowClass = { sizeof(WNDCLASSEX) };
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = G_hInstance;
		windowClass.lpszClassName = L"TheMaraning";
		RegisterClassEx(&windowClass);

		DWORD style = 0;
		DWORD styleEx = 0;

		switch (mode) {
		case WindowMode::FULLSCREEN:
			style = WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			styleEx = WS_EX_APPWINDOW;
			break;
		case WindowMode::BORDERLESS:
			style = WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			styleEx = WS_EX_APPWINDOW;
			break;
		case WindowMode::WINDOWED:
			style = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			styleEx = WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW;
			break;
		default:
			assert(false);
		}

		RECT windowRect = { 0, 0, 1280, 720 };
		AdjustWindowRectEx(&windowRect, style, FALSE, styleEx);

		G_hWnd = CreateWindowExW(
			styleEx,
			windowClass.lpszClassName,
			L"The Maraning",
			style,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr,
			nullptr,
			G_hInstance,
			nullptr);
	}

	void ProcessMessages()
	{
		MSG msg;
		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	G_hInstance = hInstance;

	setlocale(LC_ALL, ".UTF8");

	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc); // NOTE: pCmdLine doesn't contain the first argv (executable path), so we need to use GetCommandLineW()
	assert(argv);

	std::unique_ptr<std::unique_ptr<char[]>[]> argv_utf8(new std::unique_ptr<char[]>[argc]);
	for (size_t i = 0; i < argc; ++i) {
		argv_utf8[i] = std::move(WideCharToUTF8(argv[i]));
	}

	LocalFree(argv);

	return Main::Main(argc, std::move(argv_utf8));
}