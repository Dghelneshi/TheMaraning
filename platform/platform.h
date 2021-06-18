#pragma once
#include <EASTL/string.h>
#include <EASTL/string_view.h>

enum class WindowMode {
	FULLSCREEN,
	BORDERLESS,
	WINDOWED
};

#if _WIN32
#define TM_SET_THREAD_NAME(name) SetThreadDescription(GetCurrentThread(), _CRT_WIDE(name));
#else
#include <pthread.h>
#define TM_SET_THREAD_NAME(name) pthread_setname_np(pthread_self(), &name[0], sizeof(name));   
#endif

namespace Platform {
	void DebugOutput(eastl::string_view str);
	// TODO: use std::format/fmtlib instead
	void DebugOutputPrintf(const char* format, ...);

	// TODO: Window class that can be manipulated
	void CreateMainWindow(WindowMode mode);
	void SetMainWindowTitle(eastl::string_view str);

	void DisplayBlockingErrorMessage(eastl::string_view message);

	void Init();
	uint64 GetTime();
	float DeltaSeconds(uint64 begin, uint64 end = GetTime());
	void ProcessMessages();
};