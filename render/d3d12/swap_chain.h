#pragma once

#include "d3d12_includes.h"
#include "thirdparty/eastl/unique_ptr.h"

namespace D3D12
{
    // TODO: very specific to swap chain RTs for now
    struct RenderTarget
    {
        ComPtr<ID3D12Resource> D3DResource;
        D3D12_RESOURCE_STATES State = D3D12_RESOURCE_STATE_PRESENT;
    };

    struct SwapChain
    {
        void Init(HWND window, UINT backbuffer_count);
        void Shutdown();

        FORCE_INLINE UINT GetCurrentBackBufferIndex() const {
            return CurBackBufferIndex;
        }
        FORCE_INLINE void Present(UINT sync_interval, UINT flags) {
            D3DCHECKRESULT(DXGISwapChain->Present(sync_interval, flags));
            CurBackBufferIndex = DXGISwapChain->GetCurrentBackBufferIndex();
        }
        [[nodiscard]] D3D12_RESOURCE_BARRIER GetRenderTargetBarrier(D3D12_RESOURCE_STATES new_state);
    private:
        uint32 BackBufferCount;
        uint32 CurBackBufferIndex;
        ComPtr<IDXGISwapChain3> DXGISwapChain;
        eastl::unique_ptr<RenderTarget[]> RenderTargets;
    };
}