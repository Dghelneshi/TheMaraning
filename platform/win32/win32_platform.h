#pragma once

#define WIN32_LEAN_AND_MEAN

#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
//#define NOWINMESSAGES
//#define NOWINSTYLES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
//#define NOSHOWWINDOW
//#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOGDI
#define NOKERNEL
//#define NOUSER
//#define NONLS
#define NOMB
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
//#define NOMSG
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
#include "thirdparty/eastl/string.h"

namespace Platform::Win32
{
	[[nodiscard]] eastl::string WideCharToUTF8(const wchar_t* wide_string);
	[[nodiscard]] eastl::wstring UTF8ToWideChar(const char* string);
	[[nodiscard]] eastl::string WideCharToUTF8(const eastl::wstring& wide_string);
	[[nodiscard]] eastl::wstring UTF8ToWideChar(const eastl::string& string);
	[[nodiscard]] char* WideCharToUTF8Raw(const wchar_t* wide_string);
	[[nodiscard]] wchar_t* UTF8ToWideCharRaw(const char* string);
	[[nodiscard]] char* WideCharToUTF8Raw(const eastl::wstring& wide_string);
	[[nodiscard]] wchar_t* UTF8ToWideCharRaw(const eastl::string& string);

	HINSTANCE GetInstanceHandle();
	HWND GetMainWindowHandle();
};