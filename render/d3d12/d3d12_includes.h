#pragma once

#include "main/common.h"
#include "platform/win32/win32_platform.h"
#include <combaseapi.h>
#define COM_NO_WINDOWS_H
#include <d3d12.h>
#include <dxgi1_6.h>
#include "thirdparty/D3DX12/d3dx12.h"
#include "thirdparty/D3D12MemoryAllocator/D3D12MemAlloc.h"
#include <wrl/client.h>
#include <atlbase.h>

using Microsoft::WRL::ComPtr;

#if DEBUG
#define D3DCHECKRESULT(expression) HRESULT CONCAT(res, __LINE__); assert(SUCCEEDED(CONCAT(res, __LINE__) = expression))
#else
#define D3DCHECKRESULT(expression) expression
#endif

namespace D3D12
{
#ifdef DEBUG
    FORCE_INLINE void SetDebugName(IDXGIObject* pObject, const wchar_t* name) {
        pObject->SetPrivateData(WKPDID_D3DDebugObjectNameW, UINT(wcslen(name) + 1) * 2, name);
    }
    FORCE_INLINE void SetDebugName(IDXGIObject* pObject, const char* name) {
        pObject->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(strlen(name) + 1), name);
    }
    FORCE_INLINE void SetDebugName(ID3D12Object* pObject, const wchar_t* name) {
        pObject->SetName(name);
    }
    FORCE_INLINE void SetDebugName(ID3D12Object* pObject, const char* name) {
        pObject->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(strlen(name) + 1), name);
    }
#else
    FORCE_INLINE void SetDebugName(IDXGIObject*, const wchar_t*) {}
    FORCE_INLINE void SetDebugName(IDXGIObject*, const char*) {}
    FORCE_INLINE void SetDebugName(ID3D12Object*, const wchar_t*) {}
    FORCE_INLINE void SetDebugName(ID3D12Object*, const char*) {}
#endif
}