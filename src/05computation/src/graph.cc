﻿#include "computation/graph.h"
#include "hardware/device_manager.h"

namespace refactor::computation {

    Graph::Graph(graph_topo::Graph<Node, Edge> internal) noexcept
        : _internal(std::move(internal)) {}
    Graph::Graph(graph_topo::GraphTopo topology,
                 std::vector<Node> nodes,
                 std::vector<Edge> edges) noexcept
        : Graph(graph_topo::Graph<Node, Edge>{
              std::move(topology),
              std::move(nodes),
              std::move(edges),
          }) {}

    kernel::Graph Graph::lower(Target target) const {
        auto const &graph = _internal.contiguous();

        std::vector<kernel::Node> nodes(graph.nodes.size());
        std::vector<kernel::Edge> edges(graph.edges.size());

        std::unordered_map<count_t, count_t> identities;
        for (auto [nodeIdx, inputs, outputs] : graph.topology) {
            auto const &[op, name] = graph.nodes[nodeIdx];
            nodes[nodeIdx] = {nullptr, name};
            if (!op) {
                continue;
            }
            if (op->isIdentity()) {
                auto [it, ok] = identities.try_emplace(outputs[0], inputs[0]);
                ASSERT(ok, "");
                continue;
            }
            kernel::TensorRefs inputs_, outputs_;
            inputs_.reserve(inputs.size());
            outputs_.reserve(outputs.size());
            std::transform(inputs.begin(), inputs.end(),
                           std::back_inserter(inputs_), [&](auto i) {
                               return std::cref(*graph.edges[i].tensor);
                           });
            std::transform(outputs.begin(), outputs.end(),
                           std::back_inserter(outputs_), [&](auto i) {
                               return std::cref(*graph.edges[i].tensor);
                           });
            auto candidates = op->candidateKernels(target)->filter(std::move(inputs_), std::move(outputs_));
            ASSERT(!candidates.empty(), "No kernel selected for \"{}\"", name);
            nodes[nodeIdx].kernel = std::move(candidates.front());
        }

        auto device = hardware::device::fetch(hardware::Device::Type::Cpu, 0);
        for (auto i : range0_(edges.size())) {
            auto const &[tensor, name] = graph.edges[i];
            if (!tensor || identities.contains(i)) {
                edges[i] = {nullptr, 0, name};
            } else if (!tensor->data) {
                edges[i] = {nullptr, tensor->bytesSize(), name};
            } else {
                auto bytes = tensor->bytesSize();
                auto blob = device->malloc(bytes);
                blob->copyFromHost(*(tensor->data), bytes);
                edges[i] = {std::move(blob), bytes, name};
            }
        }

        auto modifier = graph_topo::InplaceModifier(graph.topology);
        modifier.reconnect(identities);
        return kernel::Graph(modifier.take(), std::move(nodes), std::move(edges));
    }

    auto Graph::internal() const -> decltype(_internal) const & { return _internal; }

}// namespace refactor::computation
