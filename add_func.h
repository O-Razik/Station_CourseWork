#ifndef ADD_FUNC_H
#define ADD_FUNC_H

#include "train.h"

std::vector<std::pair<Train *, std::pair<QTime, QTime>>>
    merge_sort_dist(std::vector<std::pair<Train*,
                    std::pair<QTime, QTime>>> array,
                    int left, int right);

#endif // ADD_FUNC_H
