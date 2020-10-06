#include "common.h"
#include "main.h"
#include "platform/platform.h"

#include "thirdparty/entt/entt.hpp"
#include "render/d3d12/d3d12_main.h"

static bool G_IsRunning = true;

namespace Main
{
	int Main(MainArgs args)
	{
		// ... process argv
		for (size_t i = 0; i < args.argc; ++i) {
			Platform::DebugOutput(args.argv[i]);
			Platform::DebugOutputA("\n");
		}
		// delete our copy of argv
		args.Destroy();

		Platform::Init();
		Platform::CreateMainWindow(WindowMode::WINDOWED);
		
		// TODO: Graphics abstraction layer.
		D3D12::RenderSystem* d3d = D3D12::RenderSystem::Get();
		d3d->Init();

		while (G_IsRunning)
		{
			uint64 frame_start = Platform::GetTime();
			
			Platform::ProcessMessages();
			
			d3d->Render();
			d3d->Present();
			
			float frame_time = Platform::DeltaSeconds(frame_start);

			char buf[128];
			snprintf(buf, sizeof(buf), "The Maraning - %.3fms\n", frame_time * 1000.0);
			Platform::SetMainWindowTitle(buf);

			Sleep(16);
		}

		d3d->Shutdown();

		return 0;
	}

	void Exit()
	{
		G_IsRunning = false;
	}
}