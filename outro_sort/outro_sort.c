#include <stddef.h>

#define SWAP(a, b) int tmp = a; a = b; b = tmp;

/******************************************************************************
 * Sort the elements of a subarray using insertion sort.
 *
 * @param arr Array.
 * @param left Lower index, inclusive.
 * @param right Higher index, exclusive.
 *****************************************************************************/
void
insertion_sort(int arr[], size_t left, size_t right)
{
    if(left + 1 >= right)
    {
        return;
    }
    for(size_t i = left + 1; i < right; ++i)
    {
        for(size_t j = i; j > left; --j)
        {
            if(arr[j - 1] > arr[j])
            {
                SWAP(arr[j - 1], arr[j])
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
    // Decrement this index, because we use the two-pointer method here, which
    // is more suited to closed intervals than half-open intervals.
    --right;
    size_t mid = (right - left) / 2 + left;
    int pivot_val = arr[mid];
    for(;; ++left, --right)
    {
        while(arr[left] < pivot_val)
        {
            ++left;
        }
        while(arr[right] > pivot_val)
        {
            --right;
        }
        if(left >= right)
        {
            return right;
        }
        SWAP(arr[left], arr[right])
    }
}

/******************************************************************************
 * Sort the elements of a subarray using outro sort. This is a hybrid algorithm
 * which executes insertion sort on small subarrays and quick sort on large
 * subarrays.
 *
 * @param arr Array.
 * @param left Lower index, inclusive.
 * @param right Higher index, exclusive.
 *****************************************************************************/
void
outro_sort(int arr[], size_t left, size_t right)
{
    if(left + 15 >= right)
    {
        insertion_sort(arr, left, right);
        return;
    }
    size_t partition_index_plus_one = partition(arr, left, right) + 1;
    outro_sort(arr, left, partition_index_plus_one);
    outro_sort(arr, partition_index_plus_one, right);
}
