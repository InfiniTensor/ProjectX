﻿# 前端图表示

前端图表示的功能是消除动态性，包括 3 种来源的动态性：

1. 来自变量形状的动态性；
2. 来自形状旁路的动态性，形状旁路指的是拓扑通向 Reshape、Expand、Squeeze、Unsqueeze 和 Slice 的子图；
3. 来自形状占位符的动态性，即 Reshape 中的 0、-1 和其他算子中常见的倒数轴表示；