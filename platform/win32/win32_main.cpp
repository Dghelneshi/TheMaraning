#include "main/common.h"
#include "platform/platform.h"
#include "win32_platform.h"
#include "main/main.h"
#include <shellapi.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>
#include <versionhelpers.h>

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

namespace EA
{
	namespace StdC
	{
		// TODO: Either use EAStdC for proper support of these functions or maybe see if we should just use std::format/fmt everywhere

		int Vsnprintf(char* __restrict pDestination, size_t n, const char* __restrict pFormat, va_list arguments) {
			return vsnprintf(pDestination, n, pFormat, arguments);
		}
		int Vsnprintf(wchar_t* __restrict pDestination, size_t n, const wchar_t* __restrict pFormat, va_list arguments) {
			return vswprintf(pDestination, n, pFormat, arguments);  // TODO: Does not conform to EAStdC spec (returns -1 if buffer is too small)
		}
	}
}

// Public Win32-specific interface
namespace Platform::Win32
{
	[[nodiscard]] eastl::string WideCharToUTF8(eastl::wstring_view wide_string) {
		return eastl::string(eastl::string::CtorConvert{}, wide_string.data(), wide_string.length());
	}
	[[nodiscard]] eastl::wstring UTF8ToWideChar(eastl::string_view string) {
		return eastl::wstring(eastl::wstring::CtorConvert{}, string.data(), string.length());
	}
	[[nodiscard]] char* WideCharToUTF8Raw(eastl::wstring_view wide_string)
	{
		int len = WideCharToMultiByte(CP_UTF8, 0, wide_string.data(), (int)wide_string.length() + 1, nullptr, 0, nullptr, nullptr);
		char* res = new char[(size_t)len];
		WideCharToMultiByte(CP_UTF8, 0, wide_string.data(), (int)wide_string.length() + 1, res, len, nullptr, nullptr);
		return res;
	}
	[[nodiscard]] wchar_t* UTF8ToWideCharRaw(eastl::string_view string)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.length() + 1, nullptr, 0);
		wchar_t* res = new wchar_t[(size_t)len];
		MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.length() + 1, res, len);
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
	void DebugOutput(eastl::string_view string) {
		thread_local eastl::wstring buf;
		buf.assign_convert(string);
		// NOTE: OutputDebugStringA doesn't support UTF8 even with everything set up correctly :(
		OutputDebugStringW(buf.data());
	}

	void DebugOutputPrintf(PRINTF_FORMAT const char* format, ...)
	{
		thread_local eastl::string buf;
		va_list args;
		va_start(args, format);
		buf.sprintf_va_list(format, args);
		va_end(args);

		// NOTE: OutputDebugStringA doesn't support UTF8 even with everything set up correctly :(
		thread_local eastl::wstring wbuf;
		wbuf.assign_convert(buf);

		OutputDebugStringW(wbuf.c_str());
	}

	void CreateMainWindow(WindowMode mode)
	{
		WNDCLASSEX windowClass = { sizeof(WNDCLASSEX) };
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = G_hInstance;
		windowClass.lpszClassName = "TheMaraning";
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

		G_hWnd = CreateWindowExA(
			styleEx,
			windowClass.lpszClassName,
			"The Maraning",
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

	void SetMainWindowTitle(eastl::string_view str) {
		SetWindowTextA(G_hWnd, str.data());
	}

	void Init() {
		LARGE_INTEGER f;
		QueryPerformanceFrequency(&f);
		G_QPCFrequencyPerSec = 1.0 / double(f.QuadPart);
	}

	uint64 GetTime() {
		LARGE_INTEGER t;
		QueryPerformanceCounter(&t);
		return (uint64)t.QuadPart;
	}

	float DeltaSeconds(uint64 begin, uint64 end) {
		return float((end - begin) * G_QPCFrequencyPerSec);
	}

	void ProcessMessages() {
		MSG msg;
		while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}

	void DisplayBlockingErrorMessage(eastl::string_view message) {
		MessageBoxA(nullptr, message.data(), "Error", MB_OK);
	}
}

#include <bcrypt.h> // NTSTATUS

using RtlGetVersionFunc = NTSTATUS (WINAPI*)(LPOSVERSIONINFOEXW);

void WindowsVersionCheck()
{
	HMODULE ntdll = GetModuleHandleA("ntdll");
	assert(ntdll);
	RtlGetVersionFunc RtlGetVersion = (RtlGetVersionFunc) GetProcAddress(ntdll, "RtlGetVersion");
	assert(RtlGetVersion);

	OSVERSIONINFOEXW osInfo { sizeof(osInfo) };
	RtlGetVersion(&osInfo);
	if (osInfo.dwMajorVersion < 10 || (osInfo.dwMajorVersion == 10 && osInfo.dwBuildNumber < 18362)) // Windows 10 1903 (build 18362) is our minimum target
	{
		Platform::DisplayBlockingErrorMessage("Windows 10 version 1903 or later is required to run this application.");
		exit(1);
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	WindowsVersionCheck();

	assert(GetACP() == CP_UTF8); // make sure the manifest worked and the Win32 API uses UTF8, works on Win10 1903 or later
	const char *locale = setlocale(LC_ALL, ".UTF8"); // enable UTF8 for the CRT, works on Win10 1803 or later
	assert(locale && (strstr(locale, ".UTF8") || strstr(locale, ".utf8") || strstr(locale, ".UTF-8") || strstr(locale, ".utf-8")));

	G_hInstance = hInstance;

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