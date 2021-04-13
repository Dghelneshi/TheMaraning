#pragma once

// TODO: make an even more lean windows.h since we have to include it all over the renderer for DX12
#define WIN32_LEAN_AND_MEAN

#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
//#define NOWINMESSAGES // message types WM_*
//#define NOWINSTYLES   // window styles WS_*
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
//#define OEMRESOURCE // includes various image/GUI related stuff if defined
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOGDI
#define NOKERNEL
//#define NOUSER // all the window-related and lots of other stuff
//#define NONLS  // WideCharToMultiByte/MultiByteToWideChar, CP_UTF8, GetACP()
//#define NOMB   // MessageBox
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
//#define NOMSG // PeekMessage, etc.
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX

#include <windows.h>

#include <EASTL/string.h>

namespace Platform::Win32
{
	[[nodiscard]] eastl::string WideCharToUTF8(eastl::wstring_view wide_string);
	[[nodiscard]] eastl::wstring UTF8ToWideChar(eastl::string_view string);
	[[nodiscard]] char* WideCharToUTF8Raw(eastl::wstring_view wide_string);
	[[nodiscard]] wchar_t* UTF8ToWideCharRaw(eastl::string_view string);

	HINSTANCE GetInstanceHandle();
	HWND GetMainWindowHandle();
};