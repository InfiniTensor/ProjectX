#ifdef USE_CUDA
#include "../src/kernels/mat_mul/cpu_kernel.hh"
#include "../src/kernels/mat_mul/cublas_kernel.hh"
#include "kernel/target.h"
#include <gtest/gtest.h>
using namespace refactor;
using namespace kernel;

TEST(kernel, MatMulCublas_OnlyBias) {
    // build routine
    auto A = Tensor::share(DataType::F32, Shape{2, 2, 2});
    auto B = Tensor::share(DataType::F32, Shape{2, 2});
    auto C = Tensor::share(DataType::F32, Shape{});
    auto Y = Tensor::share(DataType::F32, Shape{2, 2, 2});
    bool tA = false, tB = false;
    float alpha = 0.0, beta = 1.0;
    MatMulInfo info(*A, *B, *C, tA, tB, alpha, beta);
    auto kernel = MatMulCublas::build(*A, *B, *Y, info);
    ASSERT_TRUE(kernel);
    auto routine = kernel->lower();
    // malloc
    auto mfn = Target(Target::NvidiaGpu).memManager();
    auto ma = mem_manager::ForeignBlob::share(mfn, A->bytesSize());
    auto mb = mem_manager::ForeignBlob::share(mfn, B->bytesSize());
    auto mc = mem_manager::ForeignBlob::share(mfn, C->bytesSize());
    auto my = mem_manager::ForeignBlob::share(mfn, Y->bytesSize());
    // put input data
    std::vector<float> dataA{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::vector<float> dataB{0.0, 0.0, 0.0, 0.0};
    std::vector<float> dataC{2.5};
    std::vector<float> ans{2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 2.5};
    ma->copyIn(dataA.data(), A->bytesSize());
    mb->copyIn(dataB.data(), B->bytesSize());
    mc->copyIn(dataC.data(), C->bytesSize());
    // inference
    auto res = runtime::Resources();
    void const *inputs[]{*ma, *mb, *mc};
    void *outputs[]{*my};
    routine(res, inputs, outputs);
    // take output data
    std::vector<float> result(Y->elementsSize());
    my->copyOut(result.data(), Y->bytesSize());
    // check
    for (auto i = 0; i < result.size(); i++) {
        EXPECT_FLOAT_EQ(result[i], ans[i]);
    }
}

TEST(kernel, MatMulCublas_Broadcast) {
    // build routine
    auto A = Tensor::share(DataType::F32, Shape{2, 1, 2, 2});
    auto B = Tensor::share(DataType::F32, Shape{1, 2, 2, 2});
    auto C = Tensor::share(DataType::F32, Shape{2, 1});
    auto Y = Tensor::share(DataType::F32, Shape{2, 2, 2, 2});
    MatMulInfo info(*A, *B, *C);
    auto cpuKernel = MatMulCPU::build(*A, *B, *Y, info);
    auto gpuKernel = MatMulCublas::build(*A, *B, *Y, info);
    ASSERT_TRUE(cpuKernel && gpuKernel);
    auto cpuRoutine = cpuKernel->lower();
    auto gpuRoutine = gpuKernel->lower();
    // put input data
    std::vector<float> dataA{1.0, 2.0, 0.0, 0.5,
                             1.0, 0.0, 0.0, 1.0};
    std::vector<float> dataB{1.0, 2.0, 0.0, 0.5,
                             1.0, 0.0, 0.0, 1.0};
    std::vector<float> dataC{1.0, 0.0};
    std::vector<float> cpuOut(Y->elementsSize());
    auto mfn = Target(Target::NvidiaGpu).memManager();
    auto ma = mem_manager::ForeignBlob::share(mfn, A->bytesSize());
    auto mb = mem_manager::ForeignBlob::share(mfn, B->bytesSize());
    auto mc = mem_manager::ForeignBlob::share(mfn, C->bytesSize());
    auto my = mem_manager::ForeignBlob::share(mfn, Y->bytesSize());
    ma->copyIn(dataA.data(), A->bytesSize());
    mb->copyIn(dataB.data(), B->bytesSize());
    mc->copyIn(dataC.data(), C->bytesSize());
    // inference
    auto res = runtime::Resources();
    {
        void const *inputs[]{*ma, *mb, *mc};
        void *outputs[]{*my};
        gpuRoutine(res, inputs, outputs);
    }
    {
        void const *inputs[]{dataA.data(), dataB.data(), dataC.data()};
        void *outputs[]{cpuOut.data()};
        cpuRoutine(res, inputs, outputs);
    }
    // take output data
    std::vector<float> result(Y->elementsSize());
    my->copyOut(result.data(), Y->bytesSize());
    // check
    for (auto i = 0; i < result.size(); i++) {
        EXPECT_FLOAT_EQ(result[i], cpuOut[i]);
    }
}

TEST(kernel, MatMulCublas_TransABNoBias) {
    // build routine
    auto A = Tensor::share(DataType::F32, Shape{1, 3, 2, 2});
    auto B = Tensor::share(DataType::F32, Shape{2, 1, 2, 2});
    auto Y = Tensor::share(DataType::F32, Shape{2, 3, 2, 2});
    MatMulInfo info(*A, *B, true, true, 2.0);
    auto cpuKernel = MatMulCPU::build(*A, *B, *Y, info);
    auto gpuKernel = MatMulCublas::build(*A, *B, *Y, info);
    ASSERT_TRUE(cpuKernel && gpuKernel);
    auto cpuRoutine = cpuKernel->lower();
    auto gpuRoutine = gpuKernel->lower();
    // put input data
    std::vector<float> dataA{1.0, 2.0, 0.0, 0.5,
                             1.0, 0.0, 0.0, 1.0,
                             1.0, 2.0, 3.0, 4.0};
    std::vector<float> dataB{1.0, 2.0, 0.0, 0.5,
                             1.0, 0.0, 0.0, 1.0};
    std::vector<float> cpuOut(Y->elementsSize());
    auto mfn = Target(Target::NvidiaGpu).memManager();
    auto ma = mem_manager::ForeignBlob::share(mfn, A->bytesSize());
    auto mb = mem_manager::ForeignBlob::share(mfn, B->bytesSize());
    auto my = mem_manager::ForeignBlob::share(mfn, Y->bytesSize());
    ma->copyIn(dataA.data(), A->bytesSize());
    mb->copyIn(dataB.data(), B->bytesSize());
    // inference
    auto res = runtime::Resources();
    {
        void const *inputs[]{*ma, *mb};
        void *outputs[]{*my};
        gpuRoutine(res, inputs, outputs);
    }
    {
        void const *inputs[]{dataA.data(), dataB.data()};
        void *outputs[]{cpuOut.data()};
        cpuRoutine(res, inputs, outputs);
    }
    // take output data
    std::vector<float> result(Y->elementsSize());
    my->copyOut(result.data(), Y->bytesSize());
    // check
    for (auto i = 0; i < result.size(); i++) {
        EXPECT_FLOAT_EQ(result[i], cpuOut[i]);
    }
}

#endif