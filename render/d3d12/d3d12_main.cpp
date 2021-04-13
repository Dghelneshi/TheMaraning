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
		UINT dxgiFactoryFlags = 0;

#ifdef DEBUG
		// Enable the debug layer (requires the Graphics Tools "optional feature").
		// NOTE: Enabling the debug layer after device creation will invalidate the active device.
		D3DCHECKRESULT(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController)));
		{
			DebugController->EnableDebugLayer();
			DebugController->SetEnableGPUBasedValidation(true);
			DebugController->SetEnableSynchronizedCommandQueueValidation(true);

			// Enable additional debug layers.
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif


		D3DCHECKRESULT(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&Factory)));

		GetHardwareAdapter(Factory.Get(), &HardwareAdapter);

		D3DCHECKRESULT(D3D12CreateDevice(HardwareAdapter.Get(), MinFeatureLevel, IID_PPV_ARGS(&m_device)));

		if (!m_device)
			return false;

#ifdef DEBUG
		D3DCHECKRESULT(m_device.As(&DebugDevice));
		D3DCHECKRESULT(m_device.As(&DebugInfoQueue));
		//DebugInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		DebugInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		DebugInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif

		CreateAllocator();

		// Describe and create the command queue.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		D3DCHECKRESULT(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

		CreateDescriptorHeaps();

		CreateSwapChain();

		D3DCHECKRESULT(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));

		// Create the command list.
		D3DCHECKRESULT(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));

		// Command lists are created in the recording state, but there is nothing
		// to record yet. The main loop expects it to be closed, so close it now.
		D3DCHECKRESULT(m_commandList->Close());

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

		// TODO: Clean up state

#ifdef DEBUG
		DebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL);

		// Prevent another automatic report on exit.
		D3D12_MESSAGE_CATEGORY hide_categories[] = { D3D12_MESSAGE_CATEGORY_STATE_CREATION };
		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumCategories = UINT(ARRAYCOUNT(hide_categories));
		filter.DenyList.pCategoryList = hide_categories;
		DebugInfoQueue->AddStorageFilterEntries(&filter);
#endif
	}

	void RenderSystem::Render()
	{
		// Command list allocators can only be reset when the associated 
		// command lists have finished execution on the GPU; apps should use 
		// fences to determine GPU execution progress.
		D3DCHECKRESULT(m_commandAllocator->Reset());

		// However, when ExecuteCommandList() is called on a particular command 
		// list, that command list can then be reset at any time and must be before 
		// re-recording.
		D3DCHECKRESULT(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

		// Indicate that the back buffer will be used as a render target.
		D3D12_RESOURCE_BARRIER barrier = MainWindowSwapChain.GetRenderTargetBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_commandList->ResourceBarrier(1, &barrier);

		// TODO: should we directly tie the render targets to their handles instead?
		D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = RTV_Heap.GetDescriptor(MainWindowSwapChain.GetCurrentBackBufferIndex());

		// Record commands.
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		m_commandList->ClearRenderTargetView(rtv_handle, clearColor, 0, nullptr);

		// Indicate that the back buffer will now be used to present.
		barrier = MainWindowSwapChain.GetRenderTargetBarrier(D3D12_RESOURCE_STATE_PRESENT);
		m_commandList->ResourceBarrier(1, &barrier);

		D3DCHECKRESULT(m_commandList->Close());

		// Execute the command list.
		ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}

	void RenderSystem::Present()
	{
		// Present the frame.
		MainWindowSwapChain.Present(1, 0);

		WaitForPreviousFrame();
	}

	void RenderSystem::WaitForPreviousFrame()
	{
		// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
		// This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
		// sample illustrates how to use fences for efficient resource usage and to
		// maximize GPU utilization.

		// Signal and increment the fence value.
		const UINT64 fence = m_fenceValue;
		D3DCHECKRESULT(m_commandQueue->Signal(m_fence.Get(), fence));
		m_fenceValue++;

		// Wait until the previous frame is finished.
		if (m_fence->GetCompletedValue() < fence)
		{
			D3DCHECKRESULT(m_fence->SetEventOnCompletion(fence, m_fenceEvent));

			WaitForSingleObject(m_fenceEvent, INFINITE);
		}
	}


	void RenderSystem::GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter)
	{
		*ppAdapter = nullptr;

		ComPtr<IDXGIAdapter1> adapter;

		ComPtr<IDXGIFactory6> factory6;
		if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
		{
			for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;

				// Check to see whether the adapter supports Direct3D 12, but don't create the actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), MinFeatureLevel, _uuidof(ID3D12Device), nullptr)))
					break;
			}
		}
		else
		{
			UINT best_index = 0;
			SIZE_T best_mem = 0;
			for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;

				// Check to see whether the adapter supports Direct3D 12, but don't create the actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), MinFeatureLevel, _uuidof(ID3D12Device), nullptr)))
				{
					if (desc.DedicatedVideoMemory > best_mem)
					{
						best_index = adapterIndex;
						best_mem = desc.DedicatedVideoMemory;
					}
				}
			}

			D3DCHECKRESULT(pFactory->EnumAdapters1(best_index, &adapter));
		}

		*ppAdapter = adapter.Detach();
	}
}