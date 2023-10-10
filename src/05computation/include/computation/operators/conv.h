﻿#ifndef COMPUTATION_CONV_H
#define COMPUTATION_CONV_H

#include "../operator.h"
#include "kernel/collectors/pool_attributes.hh"

namespace refactor::computation {
    using kernel::PoolAttributes;

    struct Conv final : public Operator {
        PoolAttributes poolAttributes;

        explicit Conv(PoolAttributes) noexcept;

        static size_t typeId() noexcept;
        size_t opTypeId() const noexcept final;
        std::string_view name() const noexcept final;
        kernel::CollectorBox candidateKernels(Target) const noexcept final;
    };

}// namespace refactor::computation

#endif// COMPUTATION_CONV_H
