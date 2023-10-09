﻿#include "arthimetic11.hh"
#include "common/error_handler.h"
#include "common/natural.h"
#include <execution>
#include <unordered_set>

namespace refactor::kernel {
    using K = Arthimetic11;
    using Op = SimpleBinaryType;
    using DT = common::DataType;

    K::Arthimetic11(Op opType_, DT dataType_, size_t size_) noexcept
        : Kernel(), dataType(dataType_), opType(opType_), size(size_) {}

    auto K::build(Op op, Tensor const &a, Tensor const &b, Tensor const &c) noexcept -> KernelBox {
        static const std::unordered_set<decltype(DT::internal)> TYPE{
            DT::F32, DT::U8, DT::I8, DT::U16, DT::I16,
            DT::I32, DT::I64, DT::F64, DT::U32, DT::U64};
        static const std::unordered_set<Op> ARTHIMETIC{
            Op::Add, Op::Sub, Op::Mul, Op::Div};

        auto dataType = a.dataType;
        if (a.shape != b.shape ||
            a.shape != c.shape ||
            dataType != b.dataType ||
            dataType != c.dataType ||
            ARTHIMETIC.find(op) == ARTHIMETIC.end() ||
            TYPE.find(dataType.internal) == TYPE.end()) {
            return nullptr;
        }
        return std::make_unique<K>(op, dataType, a.elementsSize());
    }
    auto K::typeId() noexcept -> size_t {
        static uint8_t ID = 1;
        return reinterpret_cast<size_t>(&ID);
    }

    auto K::kernelTypeId() const noexcept -> size_t {
        return typeId();
    }
    auto K::description() const noexcept -> std::string_view {
        return "Performing add/sub/mul/div of 2 tensors of same shape on generic cpu";
    }

#define CASE_DT(OP, T)                                                   \
    case DT::T:                                                          \
        return [n = this->size](Addresses inputs, Addresses outputs) {   \
            using T_ = common::primitive_t<DT::T>::type;                 \
            auto a = static_cast<T_ const *>(inputs[0]);                 \
            auto b = static_cast<T_ const *>(inputs[1]);                 \
            auto c = static_cast<T_ *>(outputs[0]);                      \
            std::for_each_n(std::execution::par_unseq,                   \
                            common::natural_t(0), n,                     \
                            [c, a, b](auto i) { c[i] = a[i] OP b[i]; }); \
        }

#define CASE_OP(NAME, SYMBOL)        \
    case Op::NAME:                   \
        switch (dataType.internal) { \
            CASE_DT(SYMBOL, F32);    \
            CASE_DT(SYMBOL, U8);     \
            CASE_DT(SYMBOL, I8);     \
            CASE_DT(SYMBOL, U16);    \
            CASE_DT(SYMBOL, I16);    \
            CASE_DT(SYMBOL, I32);    \
            CASE_DT(SYMBOL, I64);    \
            CASE_DT(SYMBOL, F64);    \
            CASE_DT(SYMBOL, U32);    \
            CASE_DT(SYMBOL, U64);    \
            default:                 \
                UNREACHABLE();       \
        }

    auto K::lower() const noexcept -> std::function<void(Addresses, Addresses)> {
        switch (opType) {
            CASE_OP(Add, +)
            CASE_OP(Sub, -)
            CASE_OP(Mul, *)
            CASE_OP(Div, /)
            default:
                UNREACHABLE();
        }
    }

}// namespace refactor::kernel