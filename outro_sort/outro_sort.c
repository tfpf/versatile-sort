#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "outro_sort.h"

#if !defined __STDC_NO_THREADS__ && !defined __STDC_NO_ATOMICS__
#define MULTITHREADED_OUTRO_SORT
#include <stdatomic.h>
#include <threads.h>
#endif

#define WORKERS 32

struct Interval
{
    int *begin;
    int *end;
};
struct pool_t
{
    size_t head, tail;
    struct Interval work[WORKERS];
    thrd_t worker[WORKERS];
    mtx_t lock;
    cnd_t get_ok, put_ok;
};
struct completion_t
{
    size_t remaining;
    mtx_t lock;
    cnd_t exit_ok;
};

static struct pool_t pool;
static struct completion_t completion;

/******************************************************************************
 * Check whether the thread pool task list is full.
 *
 * @return
 *****************************************************************************/
bool
pool_full(void)
{
    return (pool.tail + 1) % WORKERS == pool.head;
}

/******************************************************************************
 * Check whether the thread pool task list is empty.
 *
 * @return
 *****************************************************************************/
bool
pool_empty(void)
{
    return pool.tail == pool.head;
}

/******************************************************************************
 * Remove an element from the thread pool task list. Block if necessary.
 *
 * @param interval
 *****************************************************************************/
void
pool_get(struct Interval *interval)
{
    mtx_lock(&pool.lock);
    while(pool_empty())
    {
        cnd_wait(&pool.get_ok, &pool.lock);
    }
    *interval = pool.work[pool.head];
    pool.head = (pool.head + 1) % WORKERS;
    mtx_unlock(&pool.lock);
    cnd_signal(&pool.put_ok);
printf("Got work.\n");
}

/******************************************************************************
 * Insert an element into the thread pool task list. Exit without blocking if
 * this is not possible.
 *
 * @param interval
 *
 * @return
 *****************************************************************************/
int
pool_put(struct Interval *interval)
{
    mtx_lock(&pool.lock);
    if(pool_full())
    {
printf("Could not put work.\n");
        mtx_unlock(&pool.lock);
        return -1;
    }
    pool.work[pool.tail] = *interval;
    pool.tail = (pool.tail + 1) % WORKERS;
    mtx_unlock(&pool.lock);
    cnd_signal(&pool.get_ok);
printf("Put work.\n");
    return 0;
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
 * Pick a pivot. Try to avoid the least and greatest numbers.
 *
 * @param begin Pointer to the first element.
 * @param end Pointer to one past the last element.
 *
 * @return Pivot.
 *****************************************************************************/
static int
outro_sort_pivot(int const *begin, int const *end)
{
    int const *middle = (end - 1 - begin) / 2 + begin;
    int val[] = {*begin, *middle, *(end - 1)};
    if(val[0] > val[1])
    {
        swap(val, val + 1);
    }
    if(val[0] > val[2])
    {
        swap(val, val + 2);
    }
    return val[1] < val[2] ? val[1] : val[2];
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
    int pivot_val = outro_sort_pivot(begin, end);
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
 *****************************************************************************/
static void
outro_sort_(int *begin, int *end)
{
    if(begin + 16 >= end)
    {
        insertion_sort(begin, end);
        mtx_lock(&completion.lock);
        completion.remaining -= end - begin;
        size_t remaining = completion.remaining;
        mtx_unlock(&completion.lock);
        if(remaining == 0)
        {
printf("Exit signalled.\n");
            cnd_signal(&completion.exit_ok);
        }
        return;
    }
    int *ploc = outro_sort_partition(begin, end);
    if(pool_put(&(struct Interval){.begin=begin, .end=ploc}) < 0)
    {
        outro_sort_(begin, ploc);
    }
    if(pool_put(&(struct Interval){.begin=ploc, .end=end}) < 0)
    {
        outro_sort_(ploc, end);
    }
}

/******************************************************************************
 *****************************************************************************/
static int
outro_sort_work(void *_)
{
    (void)_;
    while(true)
    {
printf("Looking for work.\n");
        struct Interval interval;
        pool_get(&interval);
printf("Picked up work.\n");
        if(interval.begin == NULL || interval.end == NULL)
        {
printf("Exiting.\n");
            thrd_exit(EXIT_SUCCESS);
        }
        outro_sort_(interval.begin, interval.end);
printf("Finished work.\n");
    }
}


static void
outro_sort_init(int const *begin, int const *end)
{
    pool.head = pool.tail = 0;
    for(int i = 0; i < WORKERS; ++i)
    {
        thrd_create(pool.worker + i, outro_sort_work, NULL);
    }
    mtx_init(&pool.lock, mtx_plain);
    cnd_init(&pool.get_ok);
    cnd_init(&pool.put_ok);

    completion.remaining = end - begin;
    mtx_init(&completion.lock, mtx_plain);
    cnd_init(&completion.exit_ok);
}

static void
outro_sort_deinit(void)
{
    mtx_lock(&completion.lock);
    while(completion.remaining != 0)
    {
        cnd_wait(&completion.exit_ok, &completion.lock);
    }
    for(int i = 0; i < WORKERS; ++i)
    {
        pool_put(&(struct Interval){.begin=NULL, .end=NULL});
    }
    for(int i = 0; i < WORKERS; ++i)
    {
        thrd_join(pool.worker[i], NULL);
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
    outro_sort_init(begin, end);
    pool_put(&(struct Interval){.begin=begin, .end=end});
    outro_sort_deinit();
}
