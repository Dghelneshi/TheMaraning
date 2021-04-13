#pragma once
#include <EASTL/string.h>
#include <EASTL/string_view.h>

enum class WindowMode {
	FULLSCREEN,
	BORDERLESS,
	WINDOWED
};

namespace Platform {
	void DebugOutput(eastl::string_view str);
	void DebugOutputPrintf(PRINTF_FORMAT const char* format, ...);

	// TODO: Window class that can be manipulated
	void CreateMainWindow(WindowMode mode);
	void SetMainWindowTitle(eastl::string_view str);

	void DisplayBlockingErrorMessage(eastl::string_view message);

	void Init();
	uint64 GetTime();
	float DeltaSeconds(uint64 begin, uint64 end = GetTime());
	void ProcessMessages();
};