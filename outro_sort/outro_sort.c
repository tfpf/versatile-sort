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
 * Configure outro sort.
 *
 * @param available_threads_ Maximum number of simultaneously active threads to
 *     use. This need not be equal to the number of logical processors;
 *     typically, using a much larger number will significantly improve
 *     performance. If multithreading is not supported, this argument is
 *     ignored.
 * @param multithreading_threshold_ Minimum size of a subarray for which
 *     multithreading should be used. If multithreading is not supported, this
 *     argument is ignored.
 *****************************************************************************/
void
outro_sort_configure(int available_threads_, size_t multithreading_threshold_)
{
#ifdef MULTITHREADED_OUTRO_SORT
    available_threads = available_threads_;
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
outro_sort_partition(int *begin, int *end)
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
outro_sort_exec(void *interval_)
{
    struct Interval *interval = interval_;
    outro_sort(interval->begin, interval->end);
    thrd_exit(EXIT_SUCCESS);
}
#endif

/******************************************************************************
 * Helper function to perform outro sort in a separate thread (if possible).
 *
 * @param begin Pointer to the first element.
 * @param end Pointer to one past the last element.
 * @param worker Thread to start.
 *
 * @return 0 if the thread was started, else -1.
 *****************************************************************************/
static int
outro_sort_dispatch(int *begin, int *end, void *thr)
{
#ifdef MULTITHREADED_OUTRO_SORT
    if(begin + multithreading_threshold <= end && available_threads > 1)
    {
        if(thrd_create(thr, outro_sort_exec, &(struct Interval){.begin=begin, .end=end}) == thrd_success)
        {
            --available_threads;
            return 0;
        }
    }
#endif
    outro_sort(begin, end);
    return -1;
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
    if(begin + 16 >= end)
    {
        insertion_sort(begin, end);
        return;
    }
    int *ploc = outro_sort_partition(begin, end);

#ifdef MULTITHREADED_OUTRO_SORT
    thrd_t worker;
#else
    int worker;
#endif
    int wstatus = outro_sort_dispatch(begin, ploc, &worker);
    outro_sort(ploc, end);

#ifdef MULTITHREADED_OUTRO_SORT
    if(wstatus == 0)
    {
        thrd_join(worker, NULL);
        ++available_threads;
    }
#endif
}
