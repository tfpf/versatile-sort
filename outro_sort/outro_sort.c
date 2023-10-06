#include <stddef.h>

#if !defined __STDC_NO_THREADS__ && !defined __STDC_NO_ATOMICS__
#define MULTITHREADED_OUTRO_SORT
#endif

#ifdef MULTITHREADED_OUTRO_SORT
#include <stdatomic.h>
#include <threads.h>
static atomic_int active_threads = 0;
#endif

void outro_sort(int *, int *);

struct Interval
{
    int *begin;
    int *end;
};

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
    int *pivot_loc = (end - 1 - begin) / 2 + begin;
    int pivot_val = *pivot_loc;
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
    thrd_exit(0);
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
    if(begin + 32768L <= ploc && active_threads < 32)
    {
        struct Interval interval = {.begin=begin, .end=ploc};
        wstatus = thrd_create(&worker, outro_sort_, &interval);
        if(wstatus == thrd_success)
        {
            ++active_threads;
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
        --active_threads;
    }
#endif
}
