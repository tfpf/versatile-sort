#define _POSIX_C_SOURCE 199309L

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITERATIONS 16

void insertion_sort(int *, int *);
void outro_sort(int *, int *);
void std_sort(int *begin, int *end);

/******************************************************************************
 * Populate the array with random values.
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
 * Check whether the array is sorted.
 *
 * @param begin Pointer to the first element.
 * @param end Pointer to one past the last element.
 *****************************************************************************/
void
verify(int const *begin, int const *end)
{
    for(int const *curr = begin + 1; curr < end; ++curr)
    {
        assert(*(curr - 1) <= *curr);
    }
}

/******************************************************************************
 * Check whether the sorting algorithm works correctly.
 *
 * @param sorter Sort function.
 * @param arr_size Number of elements to sort.
 *****************************************************************************/
void
test(void (*sorter)(int *, int *), size_t arr_size)
{
    int *arr = malloc(arr_size * sizeof *arr);
    fill(arr, arr + arr_size);
    sorter(arr, arr + arr_size);
    verify(arr, arr + arr_size);
    free(arr);
}

/******************************************************************************
 * Measure the running time of the sorting algorithm.
 *
 * @param sorter Sort function.
 * @param arr_size Number of elements to sort.
 *****************************************************************************/
void
benchmark(void (*sorter)(int *, int *), size_t arr_size)
{
    int long long nanoseconds = 0;
    int *arr = malloc(arr_size * sizeof *arr);
    for(int i = 0; i < ITERATIONS; ++i)
    {
        fill(arr, arr + arr_size);
        struct timespec start, stop;
        clock_gettime(CLOCK_REALTIME, &start);
        sorter(arr, arr + arr_size);
        clock_gettime(CLOCK_REALTIME, &stop);
        nanoseconds += stop.tv_nsec - start.tv_nsec;
        nanoseconds += 1000000000L * (stop.tv_sec - start.tv_sec);
    }
    free(arr);
    nanoseconds /= ITERATIONS;
    printf("%.3lf ms\n", nanoseconds / 1000000.0);
}

/******************************************************************************
 * Main function.
 *****************************************************************************/
int
main(int const argc, char const *argv[])
{
    size_t arr_size = 1UL << 20;
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
    test(outro_sort, arr_size);
    benchmark(outro_sort, arr_size);
    return EXIT_SUCCESS;
}
