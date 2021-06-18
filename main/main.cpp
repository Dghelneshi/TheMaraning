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
		TM_SET_THREAD_NAME("Main Thread");

		// ... process argv
		for (size_t i = 0; i < args.argc; ++i) {
			Platform::DebugOutputPrintf("%s\n", args.argv[i]);
		}
		// delete our copy of argv
		args.Destroy();
		Platform::Init();
		Platform::CreateMainWindow(WindowMode::WINDOWED);
		
		// TODO: Graphics abstraction layer.
		D3D12::RenderSystem* d3d = D3D12::RenderSystem::Get();

		if (!d3d->Init())
		{
			const char* fltext = nullptr;
			switch (D3D12::MinFeatureLevel) {
			case D3D_FEATURE_LEVEL_11_0: fltext = "11_0"; break;
			case D3D_FEATURE_LEVEL_11_1: fltext = "11_1"; break;
			case D3D_FEATURE_LEVEL_12_0: fltext = "12_0"; break;
			case D3D_FEATURE_LEVEL_12_1: fltext = "12_1"; break;
				//case D3D_FEATURE_LEVEL_12_2: fltext = "12_2"; break;
			default: TM_UNREACHABLE;
			}
			eastl::string buf(eastl::string::CtorSprintf{}, "Hardware, operating system and driver support for Direct3D 12 feature level %s is required to run this application.", fltext);
			Platform::DisplayBlockingErrorMessage(buf);
			exit(2);
		}

		while (G_IsRunning)
		{
			uint64 frame_start = Platform::GetTime();
			
			Platform::ProcessMessages();
			
			d3d->Render();
			d3d->Present();
			
			float frame_time = Platform::DeltaSeconds(frame_start);

			eastl::string buf(eastl::string::CtorSprintf{}, "The Maraning - %.3fms\n", frame_time * 1000.0);
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