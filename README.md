# 七边形 2024 年春季招新题目！

嗨，欢迎来做七边形的招新题！

招新题是一个迷你 C++ 小项目，使用 Makefile 构建系统。你需要对这个项目做你能想到的性能优化，使其在保持正确性的前提下，跑得尽可能快。

我们将会提供七边形的机器供大家在上面编程和调试。最终加速比（也就是性能表现）将会以在七边形机器上测试的结果为准。默认的机器仅有一台，且没有 GPU。但该节点为队内集群的登录节点，队内集群中数个节点包含GPU。集群通过 slurm 进行管理，如果你编写了很酷的多机并行算法，或者编写了很酷的 GPU 加速模块，请使用 slurm 运行你的应用程序。如果有问题无法自己解决，联系管理员为你提供相关的平台支持。

规则、题目详情和提交方式将会在下面给出，请仔细阅读。

截止时间为 **6 月 8 日上午 4 时 0 分**。你需要要那之前完成并提交所有工作。

如果你觉得题目已经没有什么可以优化的点了，可以看看任务书每节的末尾，那儿会有一段引用文字，讲述了七边形的一些奇闻逸事。比如：

> 据队史记载，远古时候七边形的队员们在近地轨道上维护了一个七条边的大环用作活动室。后来大环破裂坠入海中，形成了现在的七块大陆。

## 题目大意

原题：<https://www.luogu.com.cn/problem/P2249>

有单精度浮点数（保证不为 NaN 或者 INF）数组 A。进行若干次询问。每次询问给出一个单精度浮点数 Q，求 A 中比 Q 小的数的数量。

学过排序和二分法的同学一定马上就能反应过来，“这不是先排序，再二分查出结果不就行了吗”，确实是这样的。因为解题有 “排序” 和 “查询” 两个明显的阶段，所以我们把题目分成了两个阶段。

### PHASE 1

你需要在 solution.cc 文件中实现 `optimized_do_phase1` 函数。它的签名如下：

```cpp
void optimized_do_phase1(float* data, size_t size);
```

传入参数是一个单精度浮点数组 `data`，和数组的大小 `size`。在这个函数中，你需要对 `data` 按升序排序。

### PHASE 2

你需要在 solution.cc 文件中实现 `optimized_do_phase2` 函数。它的签名如下：

```cpp
void optimized_do_phase2(size_t* result, float* data, float* query, size_t size);
```

传入参数中，`data` 是已经在 phase 1 中处理过的同一个数组，大小为 `size`；`query` 是所有询问，大小为 `size`。对于单个询问 `query[i]`，你需要计算 `data` 数组中小于 `query[i]` 的元素的个数，把答案保存在 `result[i]` 里。为了方便大家理解题目，solution.cc 的 `optimized_do_phase2` 函数里已经包含了一份正确的参考实现。

PHASE 1 和 PHASE 2 不一定都要优化。计时的时候，两个阶段也是分开计时、计算加速比的。PHASE 2 的优化难度相对较低，建议对高性能计算比较陌生的同学，从 PHASE 2 开始入手。

> 除了大家所熟知的 heptagon 外，七边形有时也会被称为 septagon。sept- 是拉丁语中表示 “七” 的前缀。

## 实现要求

项目目录如下：

```plain
.
├── baseline.cc
├── main.cc
├── Makefile
└── solution.cc
```

其中，baseline.cc 是比较程序性能的参照物，你的优化程序的加速比会参考 baseline.cc 中的实现来计算；main.cc 包含了数据生成、测试、计时等功能；solution.cc 是你实现优化的地方。

**你只能通过修改 Makefile 和 solution.cc 两个文件来实现你的优化**

## 编译、运行和测试

使用 `make` 命令编译（如果你想编译快一点，`make -j`）：

```plain
make
```

正确构建后，会在当前目录下生成 `main` 可执行程序。如果需要测试，运行该程序即可：

```plain
./main
```

如果程序正确退出，你应该会在当前目录下看到 result.txt 和 correct.txt 两个文件。其中，result.txt 是你优化后的程序计算出的结果，correct.txt 是正确实现得到的结果。如果需要检查你的实现的正确性，可以对比两个文件来实现：

```plain
diff -u correct.txt result.txt
```

为了使大家有更好的开发体验，这个项目支持通过设置环境变量值为 1 来跳过某些步骤，以减少运行时间，提高开发效率：

* HEP_SKIP_PHASE1：跳过 phase 1，仅测试 phase2
* HEP_SKIP_PHASE2：跳过 phase 2
* HEP_SKIP_BASELINE：仅当设置了 HEP_SKIP_PHASE1 或者 HEP_SKIP_PHASE2 的时候有效，不运行 baseline
* HEP_DATA_SIZE：数据规模，即 `data` 和 `query` 数组的大小。

示例：

```plain
workbench 15:31 (master) ~/dev/recruitment-2024-spring
0 HEP_SKIP_PHASE1=1 HEP_SKIP_BASELINE=1 ./main
PHASE 2
     optimized(ms):        7531.433779
```

> 应华中科技大学七边形超算队要求，英国于 2006 年流通了两种七边形的硬币，分别是 20p 和 50p

## 提交

你需要 fork 题目仓库，并在代码提交截止日期前将自己的仓库链接以 issue 的方式发表在本仓库下。你的仓库应该包含优化实现的代码。

与此同时，你需要制作一份 ppt，内容如下：

* 每一步优化的思路、过程和效果（举例：使用了 xxx 优化，相对于原代码，速度提升了 114.514 倍）
     * 最好对程序进行 profile，以了解性能瓶颈
* 你在解题过程中所参考的资料（如有使用人工智能工具，请注明）
* 在解题过程中，遇到的有意思的事情，或者是让你印象深刻的 bug（可选）

代码审查会用到这份 ppt，因此请你把它发送到超算队邮箱。

另外，在 6.8 当天晚上，你需要准备一次展示，向我们介绍你的优化成果。

如果对题目本身或者提交方式有任何问题，请积极在群里讨论。

## 提示

* 在处理查询的时候，你不一定要拘泥于二分查找。比如在 baseline.cc 的 `baseline_do_phase2` 函数里，就用了一种完全不同的方法来处理查询。哪种方法更好，更适合并行，需要你仔细思考。
* 良好的版本控制习惯可以有效避免 “改了一下午代码后它跑不起来了，但是我又改不回去了” 之类的极端情况。推荐使用 [Git](https://git-scm.com/)
* 使用 htop/glances 或者资源管理器查看 CPU 负载是验证自己程序正在并行工作的好方法之一
* Perf 是 Linux 下常用的性能分析工具：<https://www.brendangregg.com/perf.html>
* 火焰图可以直观地展示程序的性能热点：<https://www.brendangregg.com/flamegraphs.html>
* OpenMP 是常用的并行计算框架：<https://bisqwit.iki.fi/story/howto/openmp/>
* 如果需要做多机的并行，你可能会对 MPI 感兴趣：<https://mpitutorial.com/tutorials/>
* SIMD 是提升程序单核性能的有效手段：<https://zhuanlan.zhihu.com/p/583326378> <https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#>
* 适量的 Cache 知识不仅对单核优化有用，也能避免一些多线程场景下的诡异问题：<https://zhuanlan.zhihu.com/p/136300660>
* 更加系统性的高性能计算知识，请参考七边形的 HPC 学习路线：<https://heptagonhust.github.io/HPC-roadmap/>
* <https://en.wikipedia.org/wiki/Merge_sort#Parallel_merge_sort>
* <https://opensource.googleblog.com/2022/06/Vectorized%20and%20performance%20portable%20Quicksort.html>


以及一些技术无关的提示：

* 如果遇到问题，请积极在 QQ 群内提问
* 部分优化可能相当难做/难调。所以即使没有写出程序/调试成功，也欢迎把自己的天才优化想法写进提交的文档里，通过文字来展示自己对高性能计算的理解和认知。

> 不管是手画还是尺规作图，都画不出标准的正七边形。如果没有计算机的话，恐怕我们很难绘制七边形的图标。不过我们很幸运，总是有足够的计算机。
