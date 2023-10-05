#define _POSIX_C_SOURCE 199309L

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void insertion_sort(int *, int *);
void outro_sort(int *, int *);

/******************************************************************************
 * Check whether the sorting algorithm works correctly.
 *
 * @param sorter Sort function.
 * @param begin Pointer to the first element.
 * @param end Pointer to one past the last element.
 *****************************************************************************/
void
test(void (*sorter)(int *, int *), int *begin, int *end)
{
    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);
    sorter(begin, end);
    clock_gettime(CLOCK_REALTIME, &stop);
    for(int const *curr = begin + 1; curr < end; ++curr)
    {
        assert(*(curr - 1) <= *curr);
    }

    int tv_sec = stop.tv_sec - start.tv_sec;
    int long tv_nsec = stop.tv_nsec - start.tv_nsec;
    if(tv_nsec < 0)
    {
        --tv_sec;
        tv_nsec += 1000000000L;
    }
    printf("%d.%06ld s\n", tv_sec, tv_nsec / 1000L);
}

/******************************************************************************
 * Populate an array with random values.
 *
 * @param begin Pointer to the first element.
 * @param end Pointer to one past the last element.
 *****************************************************************************/
void
fill(int *begin, int *end)
{
    for(int *curr = begin; curr < end; ++curr)
    {
        *curr = rand();
    }
}

/******************************************************************************
 * Main function.
 *****************************************************************************/
int
main(int const argc, char const *argv[])
{
    size_t arr_size = 1048576UL;
    if(argc > 1)
    {
        char *endptr;
        size_t arr_size_ = strtoul(argv[1], &endptr, 10);
        if(0 < arr_size_ && arr_size_ < arr_size)
        {
            arr_size = arr_size_;
        }
    }

    srand(time(NULL));
    int *arr = malloc(arr_size * sizeof *arr);
    fill(arr, arr + arr_size);
    test(outro_sort, arr, arr + arr_size);
    free(arr);
    return EXIT_SUCCESS;
}
