#pragma once

#include "d3d12_includes.h"

namespace D3D12 {

	constexpr eastl::string_view GetCommandListName(D3D12_COMMAND_LIST_TYPE type, bool queue) {
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:        return queue ? "Direct Queue"        : "Direct List";
		case D3D12_COMMAND_LIST_TYPE_BUNDLE:        return queue ? "Bundle Queue"        : "Bundle List";
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:       return queue ? "Compute Queue"       : "Compute List";
		case D3D12_COMMAND_LIST_TYPE_COPY:          return queue ? "Copy Queue"          : "Copy List";
		case D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE:  return queue ? "Video Decode Queue"  : "Video Decode List";
		case D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS: return queue ? "Video Process Queue" : "Video Process List";
		case D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE:  return queue ? "Video Encode Queue"  : "Video Encode List";
		}
		TM_UNREACHABLE;
	}

	struct CommandList
	{
		ID3D12CommandAllocator* GetAllocator()         const;
		TM_INLINE ID3D12CommandList*      GetD3DGenericCmdList() const { return D3DGenericCmdList; }
		TM_INLINE D3D12_COMMAND_LIST_TYPE GetType()              const { return Type; }
	protected:
		CommandList() = default;
		D3D12_COMMAND_LIST_TYPE Type;
		ID3D12CommandList* D3DGenericCmdList;
		ComPtr<ID3D12CommandAllocator> D3DCmdAllocator[D3D12::BackBufferCount];
	};

	// TODO: currently a bit confused since it seems the "graphics" command list is the only one that exists and it does all the work
	struct GraphicsCommandList final : public CommandList
	{
		void Init(D3D12_COMMAND_LIST_TYPE type, ID3D12PipelineState* init_state = nullptr);
		void Shutdown();

		void Reset(ID3D12PipelineState* init_state = nullptr) {
			D3DCHECKRESULT(D3DCmdList->Reset(GetAllocator(), init_state));
		}
		void BeginFrame() {
			D3DCHECKRESULT(GetAllocator()->Reset());
		}
		void Close() {
			D3DCHECKRESULT(D3DCmdList->Close());
		}
		TM_INLINE D3D12::D3D12GraphicsCommandListInterface* GetD3DGraphicsCmdList() const { return D3DCmdList.Get(); }
	private:
		ComPtr<D3D12::D3D12GraphicsCommandListInterface> D3DCmdList;
	};

	struct CommandQueue
	{
		void Init(D3D12_COMMAND_LIST_TYPE type);
		void Shutdown();

		TM_INLINE D3D12_COMMAND_LIST_TYPE GetType() const { return Type; }
		TM_INLINE ID3D12CommandQueue* GetD3DQueue() const { return D3DQueue.Get(); }
	private:
		D3D12_COMMAND_LIST_TYPE Type;
		ComPtr<ID3D12CommandQueue> D3DQueue;
	};
}