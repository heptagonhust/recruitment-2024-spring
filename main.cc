#include <cstddef>
void optimized_pre_phase1(size_t);
void optimized_do_phase1(float*, size_t);
void optimized_post_phase1();
void optimized_pre_phase2(size_t);
void optimized_do_phase2(size_t*, float*, float*, size_t);
void optimized_post_phase2();
void baseline_do_phase1(float*, size_t);
void baseline_do_phase2(size_t*, float*, float*, size_t);

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <cstring>

using namespace std::chrono_literals;

struct Config {
    bool skip_baseline;
    bool skip_phase1;
    bool skip_phase2;
    size_t data_size;

    static Config from_env(void);
};

auto Config::from_env(void) -> Config {
    Config config;
    memset(&config, 0, sizeof(config));
    if (auto opt = std::getenv("HEP_SKIP_BASELINE")) {
        config.skip_baseline = atoi(opt);
    }
    if (auto opt = std::getenv("HEP_SKIP_PHASE1")) {
        config.skip_phase1 = atoi(opt);
    }
    if (auto opt = std::getenv("HEP_SKIP_PHASE2")) {
        config.skip_phase2 = atoi(opt);
    }

    if (auto opt = std::getenv("HEP_DATA_SIZE")) {
        config.data_size = atoll(opt);
    } else {
        config.data_size = 20030601ull;
    }

    return config;
}

#define SEED 0x20030601

auto make_a_index_sequence(size_t* data, size_t data_size) {
    auto random_engine = std::default_random_engine(SEED);
    std::uniform_int_distribution<size_t> distribution(0, data_size);
    auto generator = [&distribution, &random_engine]() {
        return distribution(random_engine);
    };
    std::generate(data, data + data_size, generator);
}

auto make_a_random_sequence(float* data, size_t data_size) {
    auto random_engine = std::default_random_engine(SEED);
    std::uniform_real_distribution<float> distribution(
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::max()
    );
    auto generator = [&distribution, &random_engine]() {
        return distribution(random_engine);
    };
    std::generate(data, data + data_size, generator);
}

auto make_a_ordered_sequence(float* data, size_t data_size) {
    auto random_engine = std::default_random_engine(SEED);
    std::uniform_real_distribution<float> distribution(
        0.0,
        std::numeric_limits<float>::max() / data_size
    );
    auto start = -float(distribution(random_engine)) * (float(data_size) / 3);
    for (size_t i = 0; i < data_size; ++i) {
        data[i] = start;
        start += distribution(random_engine);
    }
}

auto dump_to_file(
    char const* filename,
    size_t const* result,
    size_t data_size
) {
    FILE* f = fopen(filename, "w");
    for (size_t i = 0; i < data_size; ++i) {
        fprintf(f, "%zu\n", result[i]);
    }
    fclose(f);
}

// 从来没有觉得写 C++ 开心过

auto do_phase1_only(Config config) {
    auto data = (float*)malloc(sizeof(float) * config.data_size);
    make_a_random_sequence(data, config.data_size);

    optimized_pre_phase1(config.data_size);
    auto optimized_begin = std::chrono::high_resolution_clock::now();
    optimized_do_phase1(data, config.data_size);
    auto optimized_end = std::chrono::high_resolution_clock::now();
    optimized_post_phase1();
    auto optimized_time = (optimized_end - optimized_begin) / 1.0ms;

    printf("PHASE 1\n");
    printf(
        "%18s: %18s\n",
        "optimized(ms)",
        std::to_string(optimized_time).c_str()
    );

    if (!config.skip_baseline) {
        auto baseline_begin = std::chrono::high_resolution_clock::now();
        baseline_do_phase1(data, config.data_size);
        auto baseline_end = std::chrono::high_resolution_clock::now();
        auto baseline_time = (baseline_end - baseline_begin) / 1.0ms;

        printf(
            "%18s: %18s\n",
            "baseline(ms)",
            std::to_string(baseline_time).c_str()
        );

        printf(
            "%18s: %18f x\n",
            "acceleration",
            double(optimized_time / baseline_time)
        );
    }
    free(data);
}

auto do_phase2_only(Config config) {
    auto data = (float*)malloc(sizeof(float) * config.data_size);
    make_a_ordered_sequence(data, config.data_size);
    auto index = (size_t*)malloc(sizeof(size_t) * config.data_size);
    make_a_index_sequence(index, config.data_size);
    auto result = (size_t*)malloc(sizeof(size_t) * config.data_size);
    auto query = (float*)malloc(sizeof(float) * config.data_size);
    for (size_t i = 0; i < config.data_size; ++i) {
        query[i] = data[index[i]];
    }
    free(index);

    optimized_pre_phase2(config.data_size);
    auto optimized_begin = std::chrono::high_resolution_clock::now();
    optimized_do_phase2(result, data, query, config.data_size);
    auto optimized_end = std::chrono::high_resolution_clock::now();
    optimized_post_phase2();
    auto optimized_time = (optimized_end - optimized_begin) / 1.0ms;

    printf("PHASE 2\n");
    printf(
        "%18s: %18s\n",
        "optimized(ms)",
        std::to_string(optimized_time).c_str()
    );

    if (!config.skip_baseline) {
        auto baseline_begin = std::chrono::high_resolution_clock::now();
        baseline_do_phase2(result, data, query, config.data_size);
        auto baseline_end = std::chrono::high_resolution_clock::now();
        auto baseline_time = (baseline_end - baseline_begin) / 1.0ms;

        printf(
            "%18s: %18s\n",
            "baseline(ms)",
            std::to_string(baseline_time).c_str()
        );

        printf(
            "%18s: %18f x\n",
            "acceleration",
            double(baseline_time / optimized_time)
        );
    }

    free(data);
    free(result);
    free(query);
}

auto do_phase1_and_phase2(Config config) {
    puts("别急，在生成数据");
    auto data = (float*)malloc(sizeof(float) * config.data_size);
    auto index = (size_t*)malloc(sizeof(size_t) * config.data_size);
    auto result = (size_t*)malloc(sizeof(size_t) * config.data_size);
    make_a_ordered_sequence(data, config.data_size);
    make_a_index_sequence(index, config.data_size);
    auto query = (float*)malloc(sizeof(float) * config.data_size);
    for (size_t i = 0; i < config.data_size; ++i) {
        query[i] = data[index[i]];
    }
    free(index);

    puts("别急，在跑优化后的代码 PHASE1");
    optimized_pre_phase1(config.data_size);
    auto optimized_phase1_begin = std::chrono::high_resolution_clock::now();
    optimized_do_phase1(data, config.data_size);
    auto optimized_phase1_end = std::chrono::high_resolution_clock::now();
    optimized_post_phase1();
    auto optimized_phase1_time =
        (optimized_phase1_end - optimized_phase1_begin) / 1.0ms;

    puts("别急，在跑优化后的代码 PHASE2");
    optimized_pre_phase2(config.data_size);
    auto optimized_phase2_begin = std::chrono::high_resolution_clock::now();
    optimized_do_phase2(result, data, query, config.data_size);
    auto optimized_phase2_end = std::chrono::high_resolution_clock::now();
    optimized_post_phase2();
    auto optimized_phase2_time =
        (optimized_phase2_end - optimized_phase2_begin) / 1.0ms;

    puts("别急，在把运行结果写盘");
    dump_to_file("result.txt", result, config.data_size);

    puts("别急，在给 baseline 生成数据");
    index = (size_t*)malloc(sizeof(size_t) * config.data_size);
    make_a_ordered_sequence(data, config.data_size);
    make_a_index_sequence(index, config.data_size);
    for (size_t i = 0; i < config.data_size; ++i) {
        query[i] = data[index[i]];
    }
    free(index);

    puts("别急，在跑 baseline PHASE1");
    auto baseline_phase1_begin = std::chrono::high_resolution_clock::now();
    baseline_do_phase1(data, config.data_size);
    auto baseline_phase1_end = std::chrono::high_resolution_clock::now();
    auto baseline_phase1_time =
        (baseline_phase1_end - baseline_phase1_begin) / 1.0ms;

    puts("别急，在跑 baseline PHASE2");
    auto baseline_phase2_begin = std::chrono::high_resolution_clock::now();
    baseline_do_phase2(result, data, query, config.data_size);
    auto baseline_phase2_end = std::chrono::high_resolution_clock::now();
    auto baseline_phase2_time =
        (baseline_phase2_end - baseline_phase2_begin) / 1.0ms;

    puts("别急，在把运行结果写盘");
    dump_to_file("correct.txt", result, config.data_size);

    puts("可以急了");

    printf("PHASE 1\n");
    printf(
        "%18s: %18s\n%18s: %18s\n%18s: %18f x\n",
        "baseline(ms)",
        std::to_string(baseline_phase1_time).c_str(),
        "optimized(ms)",
        std::to_string(optimized_phase1_time).c_str(),
        "acceleration",
        double(baseline_phase1_time / optimized_phase1_time)
    );

    printf("PHASE 2\n");
    printf(
        "%18s: %18s\n%18s: %18s\n%18s: %18f x\n",
        "baseline(ms)",
        std::to_string(baseline_phase2_time).c_str(),
        "optimized(ms)",
        std::to_string(optimized_phase2_time).c_str(),
        "acceleration",
        double(baseline_phase2_time / optimized_phase2_time)
    );

    auto baseline_time = baseline_phase1_time + baseline_phase2_time;
    auto optimized_time = optimized_phase1_time + optimized_phase2_time;
    printf("TOTALs\n");
    printf(
        "%18s: %18s\n%18s: %18s\n%18s: %18f x\n",
        "baseline(ms)",
        std::to_string(baseline_time).c_str(),
        "optimized(ms)",
        std::to_string(optimized_time).c_str(),
        "acceleration",
        double(baseline_time / optimized_time)
    );

    puts("优化后程序生成结果保存在 result.txt 中");
    puts("baseline 程序生成结果保存在 correct.txt 中");
    puts("正确性检验：使用 diff -u result.txt correct.txt");

    free(data);
    free(result);
    free(query);
}

auto main() -> int {
    auto config = Config::from_env();

    if (config.skip_phase1) {
        do_phase2_only(config);
        return 0;
    }

    if (config.skip_phase2) {
        do_phase1_only(config);
        return 0;
    }

    do_phase1_and_phase2(config);
    return 0;
}