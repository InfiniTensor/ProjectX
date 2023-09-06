﻿#include "../src/infer/infer.h"
#include <gtest/gtest.h>

using namespace refactor::common;
using namespace refactor::graph;

TEST(EdgeInfer, ArithmeticTensor) {
    {
        auto inputs = Edges{{Tensor{DataType::I32, {1, 1, 3}},
                             Tensor{DataType::I32, {2, 3}}}};
        auto result = inferArithmetic(inputs, OpType::Unknown);
        ASSERT_TRUE(result.isOk());
        ASSERT_EQ(result.unwrap(), (Edges{Tensor{DataType::I32, {1, 2, 3}}}));
    }
    {
        auto inputs = Edges{{Tensor{DataType::I32, {1, 1, 3}},
                             Tensor{DataType::I32, {1, 2}}}};
        auto result = inferArithmetic(inputs, OpType::Unknown);
        ASSERT_FALSE(result.isOk());
    }
    {
        auto inputs = Edges{{Tensor{DataType::I32, {1, 1, 3}},
                             Tensor{DataType::F32, {2, 3}}}};
        auto result = inferArithmetic(inputs, OpType::Unknown);
        ASSERT_FALSE(result.isOk());
    }
    {
        auto inputs = Edges{{Tensor{DataType::Bool, {1, 1, 3}},
                             Tensor{DataType::Bool, {2, 3}}}};
        auto result = inferArithmetic(inputs, OpType::Unknown);
        ASSERT_FALSE(result.isOk());
    }
    {
        auto inputs = Edges{{Tensor{DataType::I32, {1, 1, 3}}}};
        auto result = inferArithmetic(inputs, OpType::Unknown);
        ASSERT_FALSE(result.isOk());
    }
}

TEST(EdgeInfer, ArithmeticShapeVariable) {
    {
        auto inputs = Edges{{ShapeVariable{{1, 1, 3}},
                             ShapeVariable{{1, 2, 1}}}};
        auto result = inferArithmetic(inputs, OpType::Add);
        ASSERT_TRUE(result.isOk());
        ASSERT_EQ(result.unwrap(), (Edges{ShapeVariable{{2, 3, 4}}}));
    }
    {
        auto inputs = Edges{{ShapeVariable{{1, 1, 3}},
                             ShapeVariable{{1, 2}}}};
        auto result = inferArithmetic(inputs, OpType::Add);
        ASSERT_FALSE(result.isOk());
    }
    {
        auto inputs = Edges{{Tensor{DataType::I64, {1, 1, 3}},
                             ShapeVariable{{1, 2}}}};
        auto result = inferArithmetic(inputs, OpType::Add);
        ASSERT_FALSE(result.isOk());
    }
    {
        auto inputs = Edges{{ShapeVariable{{1, 1, 3}}}};
        auto result = inferArithmetic(inputs, OpType::Add);
        ASSERT_FALSE(result.isOk());
    }
}