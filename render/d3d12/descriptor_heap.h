#pragma once

#include "d3d12_includes.h"

namespace D3D12
{
	constexpr eastl::string_view GetDescriptorHeapName(D3D12_DESCRIPTOR_HEAP_TYPE type) {
		switch (type) {
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: return "CBV/SRV/UAV Heap";
		case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:     return "Sampler Heap";
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:         return "RTV Heap";
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:         return "DSV Heap";
		}
		TM_UNREACHABLE;
	}

	struct DescriptorHeap
	{
		void Init(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT count, bool shader_visible);
		void Shutdown();

		TM_INLINE UINT GetDescriptorSize()  const { return DescriptorSize; }
		TM_INLINE UINT GetDescriptorCount() const { return DescriptorCount; }

		struct Iterator
		{
			TM_INLINE Iterator(const DescriptorHeap& heap, size_t index = 0)
			{
				D3DHandle = heap.D3DHeap->GetCPUDescriptorHandleForHeapStart();
				DescriptorSize = heap.GetDescriptorSize();
			#ifdef DEBUG
				HeapEnd = D3DHandle.ptr + (size_t)DescriptorSize * heap.GetDescriptorCount();
			#endif
				*this += index;
			}
			TM_INLINE Iterator& operator++() {
				D3DHandle.ptr += DescriptorSize;
				return *this;
			}
			TM_INLINE Iterator operator+(int64 index_offset) {
				Iterator res = *this;
				res.D3DHandle.ptr += DescriptorSize * index_offset;
				return res;
			}
			TM_INLINE Iterator& operator+=(int64 index_offset) {
				D3DHandle.ptr += DescriptorSize * index_offset;
				return *this;
			}
			[[nodiscard]] TM_INLINE D3D12_CPU_DESCRIPTOR_HANDLE operator*() {
			#ifdef DEBUG
				TM_ASSERT(D3DHandle.ptr < HeapEnd);
			#endif
				return D3DHandle;
			}

			TM_INLINE bool operator==(const Iterator& other) { return D3DHandle.ptr == other.D3DHandle.ptr; }
			TM_INLINE bool operator!=(const Iterator& other) { return D3DHandle.ptr != other.D3DHandle.ptr; }
			TM_INLINE bool operator< (const Iterator& other) { return D3DHandle.ptr < other.D3DHandle.ptr; }

			D3D12_CPU_DESCRIPTOR_HANDLE D3DHandle;
			UINT DescriptorSize;
		#ifdef DEBUG
			SIZE_T HeapEnd;
		#endif
		};

		[[nodiscard]] TM_INLINE D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptor(size_t index) const { return *Iterator(*this, index); }
		[[nodiscard]] TM_INLINE D3D12_CPU_DESCRIPTOR_HANDLE operator[]   (size_t index) const { return *Iterator(*this, index); }
		TM_INLINE Iterator begin() const { return Iterator(*this); }
		TM_INLINE Iterator end()   const { return Iterator(*this, DescriptorCount); }

	private:
		ComPtr<ID3D12DescriptorHeap> D3DHeap;
		UINT DescriptorSize;
		UINT DescriptorCount;
		D3D12_DESCRIPTOR_HEAP_TYPE Type;
		bool IsShaderVisible;
	};
}