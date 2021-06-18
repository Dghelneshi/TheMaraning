#pragma once

#include "d3d12_includes.h"
#include "thirdparty/dxc/d3d12shader.h"
#include "thirdparty/dxc/dxcapi.h"
#include "descriptor_heap.h"
#include "swap_chain.h"
#include "command_queue.h"

namespace D3D12
{
	struct ShaderCompiler
	{
		ShaderCompiler();
	private:
		ComPtr<IDxcUtils> Utils;
		ComPtr<IDxcCompiler3> Compiler;
		ComPtr<IDxcIncludeHandler> IncludeHandler;
	};

	// TODO: - Rename
	//       - Do we really want a singleton?
	struct RenderSystem
	{
		static RenderSystem* Get() {
			static RenderSystem rs;
			return &rs;
		}

		bool Init();
		void Render();
		void Present();
		void Shutdown();

		TM_INLINE IDXGIFactory4*        GetDXGIFactory()          const { return Factory.Get(); }
		TM_INLINE ID3D12Device*         GetDevice()               const { return m_device.Get(); }
		TM_INLINE const DescriptorHeap* GetRTVDescriptorHeap()    const { return &RTV_Heap; }
		TM_INLINE const CommandQueue&	GetGraphicsCommandQueue() const { return GraphicsCommandQueue; }
		TM_INLINE D3D12MA::Allocator*   GetGlobalAllocator()      const { return GlobalAllocator; }
		TM_INLINE uint32                GetCurrentFrameIndex()    const { return CurrentFrameIndex; }
		TM_INLINE D3D_FEATURE_LEVEL     GetMaxFeatureLevel()      const { return MaxFeatureLevel; }


	private:
		RenderSystem() = default;

		void WaitForPreviousFrame();
		void CreateSwapChain();
		void CreateDescriptorHeaps();
		void CreateAllocator();
		D3D_FEATURE_LEVEL GetHardwareAdapterAndFeatureLevel(DXGIFactoryInterface* pFactory, DXGIAdapterInterface** ppAdapter);

		D3D_FEATURE_LEVEL MaxFeatureLevel = D3D_FEATURE_LEVEL_1_0_CORE;
		uint32 CurrentFrameIndex = 0;

		ComPtr<DXGIFactoryInterface> Factory;
		ComPtr<DXGIAdapterInterface> HardwareAdapter;

		D3D12MA::Allocator* GlobalAllocator;
		// TODO: split into per-frame and use queries to expire old frames (or direct reset for those interfaces that don't need state kept for all frames in flight)
		
		ShaderCompiler ShaderCompiler;
		SwapChain MainWindowSwapChain;
		DescriptorHeap RTV_Heap;
		CommandQueue GraphicsCommandQueue;
		GraphicsCommandList GraphicsCommandList;

		ComPtr<D3D12DeviceInterface> m_device;

		ComPtr<ID3D12PipelineState> m_pipelineState;
		UINT m_rtvDescriptorSize;

		// Synchronization objects
		HANDLE m_fenceEvent;
		ComPtr<ID3D12Fence1> m_fence;
		UINT64 m_fenceValue;
	};
}