﻿#include "computation/operators/dynamic_quantize_linear.h"
#include "kernel/collectors/dynamic_quantize_linear.h"

namespace refactor::computation {
    using Op = DynamicQuantizeLinear;

    size_t Op::typeId() noexcept {
        static uint8_t ID = 1;
        return reinterpret_cast<size_t>(&ID);
    }
    size_t Op::opTypeId() const noexcept { return typeId(); }
    std::string_view Op::name() const noexcept { return "DynamicQuantizeLinear"; }
    auto Op::candidateKernels(Target target) const noexcept -> kernel::CollectorBox {
        using Collector = kernel::DynamicQuantizeLinearCollector;
        return std::make_unique<Collector>(target);
    }
    auto Op::serialize() const noexcept -> std::string {
        return "DynamicQuantizeLinear()";
    }

}// namespace refactor::computation
