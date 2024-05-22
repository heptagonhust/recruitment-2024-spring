#include <cstdio>
#include <cstddef>
#include <cstdlib>

void optimized_pre_phase1(size_t) {}

void optimized_post_phase1() {}

void optimized_pre_phase2(size_t) {}

void optimized_post_phase2() {}

static int cmp(void const* a, void const* b) {
    return *(float*)a < *(float*)b ? -1 : 1;
}

void optimized_do_phase1(float* data, size_t size) {
    qsort(data, size, sizeof(data[0]), cmp);
}

void optimized_do_phase2(size_t* result, float* data, float* query, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        size_t l = 0, r = size;
        while (l < r) {
            size_t m = l + (r - l) / 2;
            if (data[m] < query[i]) {
                l = m + 1;
            } else {
                r = m;
            }
        }
        result[i] = l;
    }
}