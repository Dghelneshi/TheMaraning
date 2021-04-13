#pragma once

#include "d3d12_includes.h"
#include "thirdparty/dxc/d3d12shader.h"
#include "thirdparty/dxc/dxcapi.h"
#include "descriptor_heap.h"
#include "swap_chain.h"

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

		FORCE_INLINE IDXGIFactory4*        GetDXGIFactory()          const { return Factory.Get(); }
		FORCE_INLINE ID3D12Device*         GetDevice()               const { return m_device.Get(); }
		FORCE_INLINE const DescriptorHeap* GetRTVDescriptorHeap()    const { return &RTV_Heap; }
		FORCE_INLINE ID3D12CommandQueue*   GetGraphicsCommandQueue() const { return m_commandQueue.Get(); }
		FORCE_INLINE D3D12MA::Allocator*   GetGlobalAllocator()      const { return GlobalAllocator; }

		static constexpr D3D_FEATURE_LEVEL MinFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	private:
		RenderSystem() = default;

		void WaitForPreviousFrame();
		void CreateSwapChain();
		void CreateDescriptorHeaps();
		void CreateAllocator();
		void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter);

		static constexpr uint32 BackBufferCount = 2;

		ComPtr<IDXGIFactory4> Factory;
		ComPtr<IDXGIAdapter1> HardwareAdapter;
		D3D12MA::Allocator* GlobalAllocator;

		// Debug
		ComPtr<ID3D12Debug3> DebugController;
		ComPtr<ID3D12InfoQueue> DebugInfoQueue;
		ComPtr<ID3D12DebugDevice> DebugDevice;

		ShaderCompiler ShaderCompiler;
		SwapChain MainWindowSwapChain;
		DescriptorHeap RTV_Heap;

		ComPtr<ID3D12Device> m_device;
		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		ComPtr<ID3D12CommandQueue> m_commandQueue;
		ComPtr<ID3D12PipelineState> m_pipelineState;
		ComPtr<ID3D12GraphicsCommandList> m_commandList;
		UINT m_rtvDescriptorSize;

		// Synchronization objects
		HANDLE m_fenceEvent;
		ComPtr<ID3D12Fence> m_fence;
		UINT64 m_fenceValue;
	};
}