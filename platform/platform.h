#pragma once

enum class WindowMode {
	FULLSCREEN,
	BORDERLESS,
	WINDOWED
};

namespace Platform {
	void DebugOutputA(const char* ansi_string);
	void DebugOutput(const char* string);
	void DebugOutputPrintfA(const char* format, ...);
	void DebugOutputPrintf(const char* format, ...);
	void CreateMainWindow(WindowMode mode);
	void ProcessMessages();
};