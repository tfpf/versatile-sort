#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

// number of sorting algorithms to test
#define implemented_algorithms 4

// this should allow code portability
typedef int dtype;

// this is to create an array of functions
typedef bool (*farray)(int *, int);

////////////////////////////////////////////////////////////////////////////////

/*
	generic function to analyze performance of each sorting algorithm
	uses a pointer to a sorting function
	to analyze a particular algorithm, pass it to this function
*/
int analyze(bool sort(dtype *array, int size), dtype *array, int size)
{
	// create duplicate array for analysis
	dtype *arr_copy;
	arr_copy = malloc(size * sizeof *arr_copy);
	if(arr_copy == NULL)
	{
		printf("Ran out of memory while duplicating random array for analysis.\n");
		return 3;
	}
	memcpy(arr_copy, array, size * sizeof *arr_copy);

	// measure the time taken to sort
	bool result;
	clock_t start, stop;
	start = clock();
	result = sort(arr_copy, size);
	stop = clock();
	free(arr_copy);

	// return the number of clocks as output
	if(result)
	{
		return -1; // sorting failed
	}
	int delay;
	delay = (int)(stop - start);
	return delay;
}

////////////////////////////////////////////////////////////////////////////////

// bubble sort function
bool bubble_sort(dtype *array, int size)
{
	// preliminaries
	int x, y; // loop counters
	dtype temp; // swapper variable

	// nested loops
	for(x = size - 1; x >= 0; x--)
	{
		for(y = 0; y < x; y++)
		{
			if(array[y] > array[y + 1])
			{
				temp = array[y];
				array[y] = array[y + 1];
				array[y + 1] = temp;
			}
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

// selection sort function
bool selection_sort(dtype *array, int size)
{
	// preliminaries
	int x, y; // loop counters
	dtype temp; // swapper variable
	int min_index; // the index to swap with

	// nested loops
	for(x = 0; x < size - 1; x++)
	{
		min_index = x;
		for(y = x + 1; y < size; y++)
		{
			if(array[min_index] > array[y])
			{
				min_index = y;
			}
		}
		if(min_index != x)
		{
			temp = array[x];
			array[x] = array[min_index];
			array[min_index] = temp;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

// merge sort function
bool merge_sort(dtype *array, int size)
{
	// nothing to do if there is only one element
	if(size < 2)
	{
		return 0;
	}

	// lengths of left and right subarrays
	int L, R; // using capital L because noobs might confuse 1 and small l
	L = size >> 1;
	R = size - L;

	// allocate space for the subarrays
	dtype *left, *right;
	left = malloc(L * sizeof *left);
	right = malloc(R * sizeof *right);
	if(left == NULL || right == NULL)
	{
		printf("Ran out of memory while splitting array for merge sort.\n");
		return 4;
	}

	// assign the elements
	int count;
	for(count = 0; count < L; count++)
	{
		left[count] = array[count];
	}
	for(count = 0; count < R; count++)
	{
		right[count] = array[count + L];
	}

	// recursively sort the two subarrays
	merge_sort(left, L);
	merge_sort(right, R);

	// recombine the two sorted subarrays in ascending order
	// then release memory which is not required
	int x, y;
	x = y = 0;
	count = 0;
	while(x < L && y < R)
	{
		if(left[x] > right[y])
		{
			array[count++] = right[y++];
		}
		else
		{
			array[count++] = left[x++];
		}
	}
	while(x < L)
	{
		array[count++] = left[x++];
	}
	while(y < R)
	{
		array[count++] = right[y++];
	}
	free(left);
	free(right);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////

/*
	heap sort
	one function to make the array with respect to an index a max heap
	one function to call the above function to sort
*/

// function to make a heap
void make_heap(dtype *array, int size, int index)
{
	// mark the current element and its children
	int big, left, right;
	big = index;
	left = (index << 1) + 1;
	right = left + 1;

	// determine the largest element
	if(left < size && array[left] > array[big])
	{
		big = left;
	}
	if(right < size && array[right] > array[big])
	{
		big = right;
	}

	// largest element should be at index
	// if it is not already, make it so
	if(big != index)
	{
		dtype swapper;
		swapper = array[index];
		array[index] = array[big];
		array[big] = swapper;
		make_heap(array, size, big);
	}
	return;
}

// heap sort function
bool heap_sort(dtype *array, int size)
{
	// make heap, duh
	int count;
	for(count = (size >> 1) - 1; count >= 0; count--)
	{
		make_heap(array, size, count);
	}

	// send largest element to the end
	// then make heap again
	for(count = size - 1; count >= 0; count--)
	{
		dtype swapper;
		swapper = array[0];
		array[0] = array[count];
		array[count] = swapper;
		make_heap(array, count, 0);
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

// main
int main(int const argc, char const **argv)
{
	// check arguments
	if(argc != 2)
	{
		printf("usage:\n");
		printf("\t./sort.out <file with integers to be sorted>\n");
		return 1;
	}

	// open file
	FILE *unsorted;
	unsorted = fopen(argv[1], "r");
	if(unsorted == NULL)
	{
		printf("File \'%s\' not found.\n", argv[1]);
		return 2;
	}

	// count how many numbers need to be sorted
	int count, random;
	count = 0;
	while(!feof(unsorted))
	{
		/*
			looks complicated, but the logic is very simple
			the fscanf tries to read one integer, so maximum return value is 1
			if return value is not 1, it means scan was unsuccessfull
		*/
		count += fscanf(unsorted, "%d", &random) > 0;
	}

	// allocate space to store the numbers
	int size_in_array;
	size_in_array = count;
	int *in_array;
	in_array = malloc(size_in_array * sizeof *in_array);

	// store the numbers in this array
	rewind(unsorted);
	count = 0;
	while(count < size_in_array)
	{
		fscanf(unsorted, "%d", in_array + count++);
	}

	// create an array of functions
	farray sort_function[implemented_algorithms] =
	{
		bubble_sort,
		selection_sort,
		merge_sort,
		heap_sort
	};

	// analyze the performance of each sort function
	printf("%14s\t", "bubble sort");
	printf("%14s\t", "selection sort");
	printf("%14s\t", "merge sort");
	printf("%14s\n", "heap sort");
	for(count = 0; count < implemented_algorithms; count++)
	{
		int delay;
		delay = analyze(sort_function[count], in_array, size_in_array);
		if(delay == -1)
		{
			printf("%14s\t", "failed");
			continue;
		}
		printf("%14d\t", delay);
		fflush(stdout);
	}

	printf("\n");
	return 0;
}
