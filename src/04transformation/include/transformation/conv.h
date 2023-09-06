﻿#ifndef COMPUTE_H
#define COMPUTE_H

#include "graph/graph.h"
#include <gtest/gtest.h>
#include <map>

namespace refactor::transformation {
    class ConvTransformer {
        using GraphMutSearcher = GraphTopoSearcher<graph::Cell<graph::NodeInfo>, graph::Cell<graph::EdgeInfo>>;

    private:
        FRIEND_TEST(Transformation, Conv1x1);
        FRIEND_TEST(Transformation, DilationConv);
        enum class Rule {
            _1x1,
            Dilation,
        };

        graph::GraphMut _graph;
        std::map<GraphMutSearcher::Node, Rule> _convs;

    public:
        explicit ConvTransformer(graph::GraphMut &&graph);
        void transform();
    };

    bool MeetDilationConv(GraphTopoSearcher<graph::Cell<graph::NodeInfo>, graph::Cell<graph::EdgeInfo>>::Node node);

}// namespace refactor::transformation

#endif// COMPUTE_H