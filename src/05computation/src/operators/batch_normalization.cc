﻿#include "computation/operators/batch_normalization.h"

namespace refactor::computation {

    size_t BatchNormalization::typeId() noexcept {
        static uint8_t ID = 1;
        return reinterpret_cast<size_t>(&ID);
    }
    size_t BatchNormalization::opTypeId() const noexcept { return typeId(); }
    std::string_view BatchNormalization::name() const noexcept { return "BatchNormalization"; }

}// namespace refactor::computation