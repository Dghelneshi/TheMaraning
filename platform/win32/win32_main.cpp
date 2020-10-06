#include "main/common.h"
#include "platform/platform.h"
#include "win32_platform.h"
#include "main/main.h"
#include <shellapi.h>
#include <locale.h>

// this needs to be included in one file on windows
#include "thirdparty/rpmalloc/rpnew.h"

static HINSTANCE G_hInstance;
static HWND G_hWnd;
static double G_QPCFrequencyPerSec;

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		ValidateRect(hWnd, nullptr);
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

// Public Win32-specific interface
namespace Platform::Win32
{
	[[nodiscard]] eastl::string WideCharToUTF8(const wchar_t* wide_string) {
		return eastl::string(eastl::string::CtorConvert{}, wide_string);
	}
	[[nodiscard]] eastl::wstring UTF8ToWideChar(const char* string) {
		return eastl::wstring(eastl::wstring::CtorConvert{}, string);
	}
	[[nodiscard]] eastl::string WideCharToUTF8(const eastl::wstring& wide_string) {
		return eastl::string(eastl::string::CtorConvert{}, wide_string);
	}
	[[nodiscard]] eastl::wstring UTF8ToWideChar(const eastl::string& string) {
		return eastl::wstring(eastl::wstring::CtorConvert{}, string);
	}
	[[nodiscard]] char* WideCharToUTF8Raw(const wchar_t* wide_string)
	{
		int len = WideCharToMultiByte(CP_UTF8, 0, wide_string, -1, nullptr, 0, nullptr, nullptr);
		char* res = new char[(size_t)len + 1];
		WideCharToMultiByte(CP_UTF8, 0, wide_string, -1, res, len, nullptr, nullptr);
		res[len] = '\0';
		return res;
	}
	[[nodiscard]] wchar_t* UTF8ToWideCharRaw(const char* string)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, string, -1, nullptr, 0);
		wchar_t* res = new wchar_t[(size_t)len + 1];
		MultiByteToWideChar(CP_UTF8, 0, string, -1, res, len);
		res[len] = '\0';
		return res;
	}
	[[nodiscard]] char* WideCharToUTF8Raw(const eastl::wstring& wide_string)
	{
		int len = WideCharToMultiByte(CP_UTF8, 0, wide_string.c_str(), -1, nullptr, 0, nullptr, nullptr);
		char* res = new char[(size_t)len + 1];
		WideCharToMultiByte(CP_UTF8, 0, wide_string.c_str(), -1, res, len, nullptr, nullptr);
		res[len] = '\0';
		return res;
	}
	[[nodiscard]] wchar_t* UTF8ToWideCharRaw(const eastl::string& string)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, nullptr, 0);
		wchar_t* res = new wchar_t[(size_t)len + 1];
		MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, res, len);
		res[len] = '\0';
		return res;
	}

	HINSTANCE GetInstanceHandle() {
		return G_hInstance;
	}
	HWND GetMainWindowHandle() {
		return G_hWnd;
	}
}

// Public generic interface
namespace Platform
{
	void DebugOutputA(const char* string)
	{
		OutputDebugStringA(string);
	}
	void DebugOutput(const char* string)
	{
		auto wstr = Win32::UTF8ToWideChar(string);
		OutputDebugStringW(wstr.c_str());
	}
	void DebugOutputPrintfA(PRINTF_FORMAT const char* format, ...)
	{
		va_list args;
		char buffer[512];

		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		OutputDebugStringA(buffer);
	}
	void DebugOutputPrintf(PRINTF_FORMAT const char* format, ...)
	{
		va_list args;
		char buffer[512];

		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		auto wstr = Win32::UTF8ToWideChar(buffer);
		OutputDebugStringW(wstr.c_str());
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

	void SetMainWindowTitle(const char* str)
	{
		SetWindowTextW(G_hWnd, Win32::UTF8ToWideChar(str).c_str());
	}

	void Init()
	{
		LARGE_INTEGER f;
		QueryPerformanceFrequency(&f);
		G_QPCFrequencyPerSec = 1.0 / double(f.QuadPart);
	}

	uint64 GetTime()
	{
		LARGE_INTEGER t;
		QueryPerformanceCounter(&t);
		return (uint64)t.QuadPart;
	}

	float DeltaSeconds(uint64 begin, uint64 end)
	{
		return float((end - begin) * G_QPCFrequencyPerSec);
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

	const char** argv_utf8 = new const char*[argc];
	for (size_t i = 0; i < argc; ++i) {
		argv_utf8[i] = Platform::Win32::WideCharToUTF8Raw(argv[i]);
	}

	LocalFree(argv);

	return Main::Main(MainArgs(argc, argv_utf8));
}