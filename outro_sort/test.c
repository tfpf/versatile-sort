#define _POSIX_C_SOURCE 199309L

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void insertion_sort(int[], size_t, size_t);
void outro_sort(int[], size_t, size_t);

/******************************************************************************
 * Check whether the sorting algorithm works correctly.
 *
 * @param arr Array.
 * @param arr_size Number of elements in the array.
 * @param sorter Sort function.
 *****************************************************************************/
void
test(int arr[], size_t arr_size, void (*sorter)(int[], size_t, size_t))
{
    struct timespec begin, end;
    clock_gettime(CLOCK_REALTIME, &begin);
    sorter(arr, 0, arr_size);
    clock_gettime(CLOCK_REALTIME, &end);
    for(size_t i = 1; i < arr_size; ++i)
    {
        assert(arr[i - 1] <= arr[i]);
    }

    int tv_sec = end.tv_sec - begin.tv_sec;
    int long tv_nsec = end.tv_nsec - begin.tv_nsec;
    if(tv_nsec < 0)
    {
        --tv_sec;
        tv_nsec += 1000000000L;
    }
    printf("%d.%06ld s\n", tv_sec, tv_nsec / 1000L);
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

    srand(0);
    int *arr = malloc(arr_size * sizeof *arr);
    for(size_t i = 0; i < arr_size; ++i)
    {
        arr[i] = rand();
    }
    test(arr, arr_size, outro_sort);
    free(arr);
    return EXIT_SUCCESS;
}
