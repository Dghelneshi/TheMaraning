#pragma once

#include "d3d12_includes.h"

namespace D3D12
{
    struct DescriptorHeap
    {
        void Init(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT count, bool shader_visible);
        void Shutdown();

        FORCE_INLINE UINT GetDescriptorSize()  const { return DescriptorSize; }
        FORCE_INLINE UINT GetDescriptorCount() const { return DescriptorCount; }
        [[nodiscard]] FORCE_INLINE D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptor(size_t index) const { return *Iterator(*this, index); }
        [[nodiscard]] FORCE_INLINE D3D12_CPU_DESCRIPTOR_HANDLE operator[]   (size_t index) const { return *Iterator(*this, index); }

        struct Iterator
        {
            FORCE_INLINE Iterator(const DescriptorHeap& heap, size_t index = 0)
            {
                D3DHandle = heap.D3DHeap->GetCPUDescriptorHandleForHeapStart();
                DescriptorSize = heap.GetDescriptorSize();
            #ifdef DEBUG
                HeapEnd = D3DHandle.ptr + (size_t)DescriptorSize * heap.GetDescriptorCount();
            #endif
                *this += index;
            }
            FORCE_INLINE Iterator& operator++() {
                D3DHandle.ptr += DescriptorSize;
                return *this;
            }
            FORCE_INLINE Iterator& operator+(int64 index_offset) {
                Iterator res = *this;
                res.D3DHandle.ptr += DescriptorSize * index_offset;
                return res;
            }
            FORCE_INLINE Iterator& operator+=(int64 index_offset) {
                D3DHandle.ptr += DescriptorSize * index_offset;
                return *this;
            }
            //FORCE_INLINE operator D3D12_CPU_DESCRIPTOR_HANDLE() { assert(D3DHandle.ptr < HeapEnd); return D3DHandle; }
            [[nodiscard]] FORCE_INLINE D3D12_CPU_DESCRIPTOR_HANDLE operator*() { assert(D3DHandle.ptr < HeapEnd); return D3DHandle; }

            Iterator(const Iterator& other) = default;
            Iterator(Iterator&& other) = default;
            FORCE_INLINE Iterator& operator=(const Iterator& other) = default;
            FORCE_INLINE Iterator& operator=(Iterator&& other) = default;
            FORCE_INLINE bool operator==(const Iterator& other) { return D3DHandle.ptr == other.D3DHandle.ptr; }
            FORCE_INLINE bool operator!=(const Iterator& other) { return D3DHandle.ptr != other.D3DHandle.ptr; }
            FORCE_INLINE bool operator< (const Iterator& other) { return D3DHandle.ptr < other.D3DHandle.ptr; }

            D3D12_CPU_DESCRIPTOR_HANDLE D3DHandle;
            UINT DescriptorSize;
        #ifdef DEBUG
            SIZE_T HeapEnd;
        #endif
        };

        FORCE_INLINE Iterator begin() const { return Iterator(*this); }
        FORCE_INLINE Iterator end()   const { return Iterator(*this, DescriptorCount); }
    private:
        ComPtr<ID3D12DescriptorHeap> D3DHeap;
        UINT DescriptorSize;
        UINT DescriptorCount;
        D3D12_DESCRIPTOR_HEAP_TYPE Type;
        bool IsShaderVisible;
    };
}