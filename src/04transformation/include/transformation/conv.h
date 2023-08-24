﻿#ifndef COMPUTE_H
#define COMPUTE_H

#include "graph/graph.h"
#include <map>

namespace refactor::transformation {
    class ConvTransformer {
        using GraphMutSearcher = GraphTopoSearcher<graph::Cell<graph::NodeInfo>, graph::Cell<graph::EdgeInfo>>;

        enum class Rule {
            _1x1,
            Dilation,
        };

        graph::GraphMut _graph;
        std::map<GraphMutSearcher::Node, Rule> _convs;

    public:
        explicit ConvTransformer(graph::GraphMut &&graph);

        void transform();
        void reduce();
    };
}// namespace refactor::transformation

#endif// COMPUTE_H