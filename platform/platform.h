#pragma once

enum class WindowMode {
	FULLSCREEN,
	BORDERLESS,
	WINDOWED
};

namespace Platform {
	void DebugOutputA(const char* ansi_string);
	void DebugOutput(const char* string);
	void DebugOutputPrintfA(PRINTF_FORMAT const char* format, ...);
	void DebugOutputPrintf(PRINTF_FORMAT const char* format, ...);
	void CreateMainWindow(WindowMode mode);
	void SetMainWindowTitle(const char* str);
	void Init();
	uint64 GetTime();
	float DeltaSeconds(uint64 begin, uint64 end = GetTime());
	void ProcessMessages();
};