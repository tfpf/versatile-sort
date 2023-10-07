#include <stddef.h>
#include <stdlib.h>

#include "outro_sort.h"

#if !defined __STDC_NO_THREADS__ && !defined __STDC_NO_ATOMICS__
#define MULTITHREADED_OUTRO_SORT
#include <stdatomic.h>
#include <threads.h>
static atomic_int available_threads = 32;
static size_t multithreading_threshold = 32768U;
#endif

struct Interval
{
    int *begin;
    int *end;
};

/******************************************************************************
 * Set the number of threads to use while performing outro sort. (This need not
 * be equal to the number of logical processors; typically, using a much larger
 * number will significantly improve performance.) If multithreading is not
 * supported, this function does nothing.
 *
 * @param available_threads_
 *****************************************************************************/
void
outro_sort_set_available_threads(int available_threads_)
{
#ifdef MULTITHREADED_OUTRO_SORT
    available_threads = available_threads_;
#endif
}

/******************************************************************************
 * Set the minimum size of an array for which multithreading should be used
 * while performing outro sort. If multithreading is not supported, this
 * function does nothing.
 *
 * @param multithreading_threshold_
 *****************************************************************************/
void
outro_sort_set_multithreading_threshold(size_t multithreading_threshold_)
{
#ifdef MULTITHREADED_OUTRO_SORT
    multithreading_threshold = multithreading_threshold_;
#endif
}

/******************************************************************************
 * Exchange the integers stored at the given addresses.
 *
 * @param a
 * @param b
 *****************************************************************************/
static void
swap(int *a, int *b)
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
 * Apply Hoare's partioning scheme to a subarray: group all elements less than
 * the pivot (and possibly some elements equal to it) on one side of the pivot.
 *
 * @param begin Pointer to the first element.
 * @param end Pointer to one past the last element.
 *
 * @return Partition pointer. All elements at lower addresses will be less than
 *     or equal to the pivot.
 *****************************************************************************/
static int *
partition(int *begin, int *end)
{
    // Try not pick the least or greatest element as the pivot.
    int *middle = (end - 1 - begin) / 2 + begin;
    int val[] = {*begin, *middle, *(end - 1)};
    if(val[0] > val[1])
    {
        swap(val, val + 1);
    }
    if(val[0] > val[2])
    {
        swap(val, val + 2);
    }
    if(val[1] > val[2])
    {
        swap(val + 1, val + 2);
    }
    int pivot_val = val[1];

    for(;; ++begin, --end)
    {
        while(*begin < pivot_val)
        {
            ++begin;
        }
        while(*(end - 1) > pivot_val)
        {
            --end;
        }
        if(begin + 1 >= end)
        {
            return end;
        }
        swap(begin, end - 1);
    }
}

#ifdef MULTITHREADED_OUTRO_SORT
/******************************************************************************
 * Helper function to perform outro sort.
 *
 * @param interval_ Sort range.
 *
 * @return Ignored.
 *****************************************************************************/
static int
outro_sort_(void *interval_)
{
    struct Interval *interval = interval_;
    outro_sort(interval->begin, interval->end);
    thrd_exit(EXIT_SUCCESS);
}
#endif

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
    if(begin + 16 >= end)
    {
        insertion_sort(begin, end);
        return;
    }
    int *ploc = partition(begin, end);

    // First recursive call in a separate thread (if possible).
#ifdef MULTITHREADED_OUTRO_SORT
    thrd_t worker;
    int wstatus = thrd_error;
    if(begin + multithreading_threshold <= ploc && available_threads > 1)
    {
        struct Interval interval = {.begin=begin, .end=ploc};
        wstatus = thrd_create(&worker, outro_sort_, &interval);
        if(wstatus == thrd_success)
        {
            --available_threads;
        }
    }
    if(wstatus != thrd_success)
#endif
    {
        outro_sort(begin, ploc);
    }

    // Second recursive call in the current thread.
    outro_sort(ploc, end);

    // Join the other thread.
#ifdef MULTITHREADED_OUTRO_SORT
    if(wstatus == thrd_success)
    {
        thrd_join(worker, NULL);
        ++available_threads;
    }
#endif
}
