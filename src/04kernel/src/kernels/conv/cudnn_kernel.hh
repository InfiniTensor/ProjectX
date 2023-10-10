﻿#ifndef KERNEL_CONV_CUDNN_KERNEL_HH
#define KERNEL_CONV_CUDNN_KERNEL_HH

#include "cudnn_impl.h"
#include "kernel/kernel.h"

namespace refactor::kernel {

    struct ConvCudnn final : public Kernel {

        ConvCudnn() noexcept;

        static KernelBox build() noexcept;
        static size_t typeId() noexcept;

        size_t kernelTypeId() const noexcept final;
        std::string_view description() const noexcept final;
        Operation lower() const noexcept final;
    };

}// namespace refactor::kernel

#endif// KERNEL_CONV_CUDNN_KERNEL_HH