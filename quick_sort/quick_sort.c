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
 * Picking a suitable element (called pivot) a of a subarray, group all
 * elements less than it on one side and all elements greater than it on the
 * other.
 *
 * @param arr Array.
 * @param left Lower index, inclusive.
 * @param right Higher index, exclusive.
 *
 * @return Index of the pivot.
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
 * Sort the elements of a subarray using quick sort.
 *
 * @param arr Array.
 * @param left Lower index, inclusive.
 * @param right Higher index, exclusive.
 *****************************************************************************/
void
quick_sort(int arr[], size_t left, size_t right)
{
    if(left + 1 >= right)
    {
        return;
    }
    if(left + 15 >= right)
    {
        insertion_sort(arr, left, right);
        return;
    }
    size_t pivot_index_plus_one = partition(arr, left, right) + 1;
    quick_sort(arr, left, pivot_index_plus_one);
    quick_sort(arr, pivot_index_plus_one, right);
}
