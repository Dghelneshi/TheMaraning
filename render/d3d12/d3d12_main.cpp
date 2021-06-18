#include "d3d12_main.h"
#include "platform/platform.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "dxguid.lib")

namespace D3D12
{
	ShaderCompiler::ShaderCompiler()
	{
		// Create compiler and utils.
		D3DCHECKRESULT(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&Utils)));
		D3DCHECKRESULT(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&Compiler)));

		// Create default include handler. (You can create your own...)
		D3DCHECKRESULT(Utils->CreateDefaultIncludeHandler(&IncludeHandler));
	}

	bool RenderSystem::Init()
	{
		if (m_device)
			Shutdown();

		UINT dxgiFactoryFlags = 0;

#ifdef DEBUG
		{
			// Enable the debug layer (requires the Graphics Tools "optional feature").
			// NOTE: Enabling the debug layer after device creation will invalidate the active device.
			ComPtr<ID3D12Debug3> DebugController;
			D3DCHECKRESULT(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController)));
			DebugController->EnableDebugLayer();
			DebugController->SetEnableGPUBasedValidation(true);
			DebugController->SetEnableSynchronizedCommandQueueValidation(true);

			// Enable additional debug layers.
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif

		D3DCHECKRESULT(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&Factory)));
		
		MaxFeatureLevel = GetHardwareAdapterAndFeatureLevel(Factory.Get(), &HardwareAdapter);

		D3DCHECKRESULT(D3D12CreateDevice(HardwareAdapter.Get(), MaxFeatureLevel, IID_PPV_ARGS(&m_device)));

		if (!m_device)
			return false;

#ifdef DEBUG
		{
			ComPtr<ID3D12InfoQueue> DebugInfoQueue;
			D3DCHECKRESULT(m_device.As(&DebugInfoQueue));
			//DebugInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			DebugInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			DebugInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		}
#endif

		CreateAllocator();

		GraphicsCommandQueue.Init(D3D12_COMMAND_LIST_TYPE_DIRECT);
		GraphicsCommandList.Init(D3D12_COMMAND_LIST_TYPE_DIRECT);

		CreateDescriptorHeaps();

		CreateSwapChain();

		// Command lists are created in the recording state, but there is nothing
		// to record yet. The main loop expects it to be closed, so close it now.
		GraphicsCommandList.Close();

		// Create synchronization objects.
		{
			D3DCHECKRESULT(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
			m_fenceValue = 1;

			// Create an event handle to use for frame synchronization.
			m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (m_fenceEvent == nullptr)
			{
				D3DCHECKRESULT(HRESULT_FROM_WIN32(GetLastError()));
			}
		}
		return true;
	}

	void RenderSystem::CreateAllocator()
	{
		D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
		allocatorDesc.pDevice = m_device.Get();
		allocatorDesc.pAdapter = HardwareAdapter.Get();

		D3DCHECKRESULT(D3D12MA::CreateAllocator(&allocatorDesc, &GlobalAllocator));
	}
	void RenderSystem::CreateDescriptorHeaps()
	{
		RTV_Heap.Init(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, BackBufferCount, false);
	}
	void RenderSystem::CreateSwapChain()
	{
		MainWindowSwapChain.Init(Platform::Win32::GetMainWindowHandle(), BackBufferCount);
	}

	void RenderSystem::Shutdown()
	{
		// Ensure that the GPU is no longer referencing resources that are about to be cleaned up by the destructor.
		WaitForPreviousFrame();

		CloseHandle(m_fenceEvent);
		m_fence.Reset();
		GlobalAllocator->Release();
		m_pipelineState.Reset();
		RTV_Heap.Shutdown();
		MainWindowSwapChain.Shutdown();
		GraphicsCommandList.Shutdown();
		GraphicsCommandQueue.Shutdown();
		Factory.Reset();
		HardwareAdapter.Reset();

#ifdef DEBUG
		{
			ComPtr<ID3D12DebugDevice> DebugDevice;
			D3DCHECKRESULT(m_device.As(&DebugDevice));
			DebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);

			ComPtr<ID3D12InfoQueue> DebugInfoQueue;
			D3DCHECKRESULT(m_device.As(&DebugInfoQueue));
			// Prevent another automatic report on exit.
			D3D12_MESSAGE_CATEGORY hide_categories[] = { D3D12_MESSAGE_CATEGORY_STATE_CREATION };
			D3D12_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumCategories = UINT(ARRAYCOUNT(hide_categories));
			filter.DenyList.pCategoryList = hide_categories;
			DebugInfoQueue->AddStorageFilterEntries(&filter);
		}
#endif
		m_device.Reset();
	}

	void RenderSystem::Render()
	{
		GraphicsCommandList.BeginFrame();
		GraphicsCommandList.Reset(m_pipelineState.Get());

		D3D12GraphicsCommandListInterface* cmdlist = GraphicsCommandList.GetD3DGraphicsCmdList();

		// Indicate that the back buffer will be used as a render target.
		D3D12_RESOURCE_BARRIER barrier = MainWindowSwapChain.GetRenderTargetBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET);
		cmdlist->ResourceBarrier(1, &barrier);

		// TODO: should we directly tie the render targets to their handles instead?
		D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = RTV_Heap.GetDescriptor(MainWindowSwapChain.GetCurrentBackBufferIndex());

		// Record commands.
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		cmdlist->ClearRenderTargetView(rtv_handle, clearColor, 0, nullptr);

		// Indicate that the back buffer will now be used to present.
		barrier = MainWindowSwapChain.GetRenderTargetBarrier(D3D12_RESOURCE_STATE_PRESENT);
		cmdlist->ResourceBarrier(1, &barrier);

		GraphicsCommandList.Close();

		// Execute the command list.
		ID3D12CommandList* ppCommandLists[] = { cmdlist };
		GraphicsCommandQueue.GetD3DQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}

	void RenderSystem::Present()
	{
		// Present the frame.
		MainWindowSwapChain.Present(1, 0);

		WaitForPreviousFrame();
		CurrentFrameIndex = (CurrentFrameIndex + 1) % BackBufferCount;
	}

	void RenderSystem::WaitForPreviousFrame()
	{
		// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
		// This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
		// sample illustrates how to use fences for efficient resource usage and to
		// maximize GPU utilization.

		// Signal and increment the fence value.
		const UINT64 fence = m_fenceValue;
		D3DCHECKRESULT(GraphicsCommandQueue.GetD3DQueue()->Signal(m_fence.Get(), fence));
		m_fenceValue++;

		// Wait until the previous frame is finished.
		if (m_fence->GetCompletedValue() < fence)
		{
			D3DCHECKRESULT(m_fence->SetEventOnCompletion(fence, m_fenceEvent));

			WaitForSingleObject(m_fenceEvent, INFINITE);
		}
	}


	D3D_FEATURE_LEVEL RenderSystem::GetHardwareAdapterAndFeatureLevel(DXGIFactoryInterface* pFactory, DXGIAdapterInterface** ppAdapter)
	{
		ComPtr<DXGIAdapterInterface> adapter;

		// NOTE: new way of enumerating adapters since IDXGIFactory6, 1803
		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;

			ComPtr<D3D12DeviceInterface> device;

			// Check to see whether the adapter supports Direct3D 12, but don't create the actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), MinFeatureLevel, IID_PPV_ARGS(&device))))
			{
				constexpr D3D_FEATURE_LEVEL levels[] = {
					D3D_FEATURE_LEVEL_11_0,
					D3D_FEATURE_LEVEL_11_1,
					D3D_FEATURE_LEVEL_12_0,
					D3D_FEATURE_LEVEL_12_1,
					//D3D_FEATURE_LEVEL_12_2
				};
				D3D12_FEATURE_DATA_FEATURE_LEVELS fl_info{};
				fl_info.NumFeatureLevels = (UINT)ARRAYCOUNT(levels);
				fl_info.pFeatureLevelsRequested = levels;
				D3DCHECKRESULT(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &fl_info, sizeof(fl_info)));
				*ppAdapter = adapter.Detach();
				return fl_info.MaxSupportedFeatureLevel;
			}
		}
		return D3D_FEATURE_LEVEL_1_0_CORE;
	}
}