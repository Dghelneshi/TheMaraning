#include "common.h"
#include "main.h"
#include "platform/platform.h"

static bool G_IsRunning = true;

namespace Main
{
	int Main(int argc, std::unique_ptr<std::unique_ptr<char[]>[]> argv)
	{
		// ... process argv
		for (size_t i = 0; i < argc; ++i) {
			Platform::DebugOutput(argv[i].get());
			Platform::DebugOutput("\n");
		}
		// delete our copy of argv
		argv.reset();

		Platform::CreateMainWindow(WindowMode::WINDOWED);

		while (G_IsRunning)
		{
			Platform::ProcessMessages();
		}

		return 0;
	}

	void Exit()
	{
		G_IsRunning = false;
	}
}