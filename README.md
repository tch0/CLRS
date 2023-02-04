# 算法导论中典型算法的平凡实现

说明：
- 包括一些典型算法。
- 不追求性能，仅要求正确，实现非常平凡。
- 使用C++实现，尽可能使用模板泛化。
- 仅头文件。

测试：
```shell
cd ./test
make run
```

索引：
- 分治策略
    - 矩阵乘法的Strassen算法
- 排序
    - [插入排序](./include/Sorting.hpp#L13)
    - [归并排序](./include/Sorting.hpp#L52)
    - [快速排序](./include/Sorting.hpp#L79)
    - [堆排序](./include/Sorting.hpp#L69)
    - [计数排序](./include/Sorting.hpp#L93)
    - 基数排序
    - 桶排序
- 数据结构
    - 哈希表
- 动态规划
    - [最大子数组问题](./include/DP.hpp#L17)
    - [矩阵链乘法](./include/DP.hpp#L45)
    - [最长公共子序列](./include/DP.hpp#L109)
    - 最优二叉搜索树
- 贪心
    - 赫夫曼编码
- 树
    - [二叉搜索树](./include/Tree.hpp)
    - [红黑树](./include/TreeImpl_RbTree.hpp)
    - AVL树
    - [树堆（treap）](./include/TreeImpl_Treap.hpp)
    - [顺序统计树（order statistic tree）](./include/TreeImpl_OrderStatisticTree.hpp)
    - 跳表（skip list）
    - 伸展树（splay tree）
    - 左偏树（leftist tree）
    - 线段树（segment tree）
    - 区间树（interval tree）
    - B树
    - B+树
    - 斐波那契堆
    - Van Emde Boas 树
    - 不相交集
- 图
    - 深度优先搜索
    - 广度优先搜索
    - 强联通分量
    - 最小生成树
        - Kruskal
        - Prim
    - 单源最短路径
        - Bellman-Ford
        - Dijkstra
    - 节点对最短路径
        - Floyd-Warshall
        - 稀疏图Johnson
    - 最大流
        - Ford-Fulkerson
        - 最大二分匹配
- 矩阵
    - 求解线性方程组
    - 矩阵求逆
    - 对称正定矩阵
    - 最小二乘逼近
- 线性规划
- 快速傅里叶变换
- 数论
    - 最大公约数
    - 模线性方程
    - RSA
- 字符串匹配
    - 朴素字符串匹配
    - Rabin-Karp
    - 有限自动机
    - KMP
- 计算几何
    - 线段相交
    - 凸包
    - 最近点对
