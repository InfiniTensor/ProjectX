﻿#include "infer.h"
#include <unordered_set>

namespace refactor::onnx {
    using namespace refactor::common;

    InferResult inferUnsqueeze(Operator const &op, Tensors inputs) {
        auto const &data = inputs[0];
        auto const &axes = inputs[1];
        if (axes->dataType != DataType::I64 || axes->shape.size() != 1 || !axes->hasData()) {
            return Err(InferError(ERROR_MSG("Axes not support")));
        }
        auto rank = data->shape.size();
        auto axes_ = reinterpret_cast<int64_t *>(axes->data->ptr);
        EXPECT_VAL(axes->shape[0], axesSize)
        std::unordered_set<int64_t> axes__;
        for (auto ptr = axes_; ptr != axes_ + axesSize; ++ptr) {
            auto axis = *ptr;
            if (axis < 0) {
                axis += rank;
            }
            if (axis < 0 || rank < axis) {
                return Err(InferError(ERROR_MSG("Axes out of range")));
            }
            axes__.insert(axis);
        }
        ASSERT(axes__.size() == axesSize, "Axes has duplicate");
        Shape output(rank + axesSize, DimExpr(1));
        auto it = data->shape.begin();
        for (size_t i = 0; i < output.size(); ++i) {
            if (!axes__.erase(i)) {
                output[i] = *it++;
            }
        }
        return Ok(Tensors{std::make_shared<Tensor>(data->dataType, std::move(output), data->data)});
    }
}// namespace refactor::onnx