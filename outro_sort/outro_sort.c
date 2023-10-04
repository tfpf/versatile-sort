#include <stddef.h>

#define SWAP(a, b) int tmp = a; a = b; b = tmp;

/******************************************************************************
 * Exchange the integers stored at the given addresses.
 *
 * @param a
 * @param b
 *****************************************************************************/
void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

/******************************************************************************
 * Sort the elements of a subarray using insertion sort.
 *
 * @param begin Pointer to the first element.
 * @param end Pointer to one past the last element.
 *****************************************************************************/
void
insertion_sort(int *begin, int *end)
{
    if(begin + 1 >= end)
    {
        return;
    }
    for(int *curr = begin + 1; curr < end; ++curr)
    {
        for(int *gap = curr; gap > begin; --gap)
        {
            if(*(gap - 1) > *gap)
            {
                swap(gap - 1, gap);
            }
        }
    }
}

/******************************************************************************
 * Apply Hoare's partioning scheme: group all elements less than the pivot (and
 * possibly some elements equal to it) on one side of the pivot.
 *
 * @param arr Array.
 * @param left Lower index, inclusive.
 * @param right Higher index, exclusive.
 *
 * @return Partition index. All elements at lower indices will be less than or
 *     equal to the pivot.
 *****************************************************************************/
size_t
partition(int arr[], size_t left, size_t right)
{
    size_t mid = (right - 1 - left) / 2 + left;
    int pivot_val = arr[mid];
    for(;; ++left, --right)
    {
        while(arr[left] < pivot_val)
        {
            ++left;
        }
        while(arr[right - 1] > pivot_val)
        {
            --right;
        }
        if(left + 1 >= right)
        {
            return right;
        }
        SWAP(arr[left], arr[right - 1])
    }
}

/******************************************************************************
 * Sort the elements of a subarray using outro sort. This is a hybrid algorithm
 * which executes insertion sort on small subarrays and quick sort on large
 * subarrays.
 *
 * @param begin Pointer to the first element.
 * @param end Pointer to one past the last element.
 *****************************************************************************/
void
outro_sort(int *begin, int *end)
{
    if(begin + 15 >= end)
    {
        insertion_sort(begin, end);
        return;
    }
    size_t partition_index = partition(begin, 0, end - begin);
    outro_sort(begin, begin + partition_index);
    outro_sort(begin + partition_index, end);
}
