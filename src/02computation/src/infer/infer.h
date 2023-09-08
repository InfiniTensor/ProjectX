﻿#ifndef GRAPH_INFER_H
#define GRAPH_INFER_H

#include "common/error_handler.h"
#include "graph/graph.h"
#include <result.h>

namespace refactor::graph {

    using ShapeOrNot = std::optional<Shape>;
    using Edges = std::vector<Edge>;

    struct InferError : public std::runtime_error {
        explicit InferError(std::string &&msg);
    };
    using InferResult = Result<Edges, InferError>;

#define ERROR_MSG(msg) buildMsg(msg, __FILE__, __LINE__)

    InferResult inferUnary(Operator const &, Edges);
    InferResult inferArithmetic(Operator const &, Edges);
    InferResult inferGemm(Operator const &, Edges);
    InferResult inferMatMul(Operator const &, Edges);
    InferResult inferReshape(Operator const &, Edges);
    InferResult inferCumSum(Operator const &, Edges);
    InferResult inferSlice(Operator const &, Edges);
    InferResult inferShape(Operator const &, Edges);
    InferResult inferWhere(Operator const &, Edges);
    InferResult inferSqueeze(Operator const &, Edges);
    InferResult inferEqual(Operator const &, Edges);
    InferResult inferSoftmax(Operator const &, Edges);
    InferResult inferPow(Operator const &, Edges);
    InferResult inferReduce(Operator const &, Edges);
    InferResult inferConcat(Operator const &, Edges);
    InferResult inferGather(Operator const &, Edges);
    InferResult inferCast(Operator const &, Edges);
    InferResult inferMax(Operator const &, Edges);
    InferResult inferTranspose(Operator const &, Edges);
    InferResult inferExpand(Operator const &, Edges);

    using ShapeResult = Result<Shape, std::string>;

    /// @brief 多方向形状广播。
    /// @param inputs 所有输入的形状。
    /// @return 广播后的形状。
    ShapeResult multidirBroadcast(std::vector<Shape> const &);

    /// @brief 单方向形状广播。
    /// @param target 目标形状。
    /// @param test 测试形状。
    /// @return 测试形状是否可以广播到目标形状。
    bool unidirBroadcast(Shape const &target, Shape const &test);

    /// @brief 池化形状推断。
    /// @param data 输入张量的形状。
    /// @param kernel kernel 的形状。
    /// @param dilations 空洞参数。
    /// @param pads 扩张参数。
    /// @param strides 跳步参数。
    /// @return 池化后的形状。
    ShapeResult pool(Shape const &data,
                     Shape const &kernel,
                     ShapeOrNot const &dilations,
                     ShapeOrNot const &pads,
                     ShapeOrNot const &strides);

}// namespace refactor::graph

#endif// GRAPH_INFER_H