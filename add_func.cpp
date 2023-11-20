#include "add_func.h"

std::vector<std::pair<Train*, std::pair<QTime, QTime>>> merge_sort_dist
    (std::vector<std::pair<Train*, std::pair<QTime, QTime>>> array, int left, int right)
{
    if (left >= right)
    {
        std::vector<std::pair<Train*, std::pair<QTime, QTime>>> single_element_array = {array[left]};
        return single_element_array;
    }

    int middle = left + (right - left) / 2;

    std::vector<std::pair<Train*, std::pair<QTime, QTime>>> left_half =
        merge_sort_dist(array, left, middle);
   std::vector<std::pair<Train*, std::pair<QTime, QTime>>> right_half =
        merge_sort_dist(array, middle + 1, right);

    std::vector<std::pair<Train*, std::pair<QTime, QTime>>> merge_array;

    int i = 0, j = 0;
    while (i < left_half.size() && j < right_half.size())
    {
        if (left_half[i].first->get_distance() < right_half[j].first->get_distance())
        {
            merge_array.push_back(left_half[i]);
            i++;
        }
        else
        {
            merge_array.push_back(right_half[j]);
            j++;
        }
    }

    while (i < left_half.size())
    {
        merge_array.push_back(left_half[i]);
        i++;
    }

    while (j < right_half.size())
    {
        merge_array.push_back(right_half[j]);
        j++;
    }

    return merge_array;
}

