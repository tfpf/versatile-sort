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
static void
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

/******************************************************************************
 * Helper function to perform outro sort.
 *
 * @param arg_ Sort range.
 *
 * @return Ignored.
 *****************************************************************************/
int
outro_sort_wrapper(void *arg_)
{
    struct Interval *arg = arg_;
    outro_sort(arg->begin, arg->end);
    thrd_exit(0);
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
    int *ploc = partition(begin, end);
#ifdef MULTITHREADED_OUTRO_SORT
    if(begin + 32768L <= end && active_threads < 64)
    {
        struct Interval lt_interval = {.begin=begin, .end=ploc};
        struct Interval rt_interval = {.begin=ploc, .end=end};
        thrd_t lt, rt;
        active_threads += 2;
        int lt_status = thrd_create(&lt, outro_sort_wrapper, &lt_interval);
        int rt_status = thrd_create(&rt, outro_sort_wrapper, &rt_interval);
        lt_status == thrd_success ? thrd_join(lt, NULL) : outro_sort(begin, ploc);
        rt_status == thrd_success ? thrd_join(rt, NULL) : outro_sort(ploc, end);
        active_threads -= 2;
    }
    else
#endif
    {
        outro_sort(begin, ploc);
        outro_sort(ploc, end);
    }
}
