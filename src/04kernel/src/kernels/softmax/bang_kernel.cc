#include "bang_kernel.hh"

namespace refactor::kernel {
    using K = SoftmaxBang;

    K::SoftmaxBang(SoftmaxInfo info_) noexcept
        : Kernel(), info(std::move(info_)) {}

    auto K::build(SoftmaxInfo info) noexcept -> KernelBox {
#ifndef USE_BANG
        return nullptr;
#endif

        return info.type.isFloat()
                   ? std::make_unique<K>(std::move(info))
                   : nullptr;
    }

    auto K::typeId() noexcept -> size_t {
        static uint8_t ID = 1;
        return reinterpret_cast<size_t>(&ID);
    }

    auto K::kernelTypeId() const noexcept -> size_t { return typeId(); }
    auto K::description() const noexcept -> std::string_view {
        return "Performing Softmax using BANG";
    }

}// namespace refactor::kernel
