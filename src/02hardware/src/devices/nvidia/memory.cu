﻿#include "common.h"
#include "memory.cuh"

#define CUDA_ASSERT(STATUS)                                                 \
    if (auto status = (STATUS); status != cudaSuccess) {                    \
        RUNTIME_ERROR(fmt::format("cuda failed on \"" #STATUS "\" with {}", \
                                  cudaGetErrorString(status)));             \
    }

namespace refactor::hardware {
    using M = NvidiaMemory;

    void *M::malloc(size_t size) noexcept {
        void *ptr;
        CUDA_ASSERT(cudaMalloc(&ptr, size));
        return ptr;
    }
    void M::free(void *ptr) noexcept {
        CUDA_ASSERT(cudaFree(ptr));
    }
    void *M::copyHD(void *dst, void const *src, size_t bytes) const noexcept {
        CUDA_ASSERT(cudaMemcpy(dst, src, bytes, cudaMemcpyHostToDevice));
    }
    void *M::copyDH(void *dst, void const *src, size_t bytes) const noexcept {
        CUDA_ASSERT(cudaMemcpy(dst, src, bytes, cudaMemcpyDeviceToHost));
    }
    void *M::copyDD(void *dst, void const *src, size_t bytes) const noexcept {
        CUDA_ASSERT(cudaMemcpy(dst, src, bytes, cudaMemcpyDeviceToDevice));
    }

}// namespace refactor::hardware