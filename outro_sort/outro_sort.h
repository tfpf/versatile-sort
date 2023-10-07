#ifndef TFPF_VERSATILE_SORT_OUTRO_SORT_OUTRO_SORT_H_
#define TFPF_VERSATILE_SORT_OUTRO_SORT_OUTRO_SORT_H_

#include <stddef.h>

void insertion_sort(int *, int *);
void outro_sort(int *, int *);
void outro_sort_set_available_threads(int);
void outro_sort_set_multithreading_threshold(size_t);

#endif  // TFPF_VERSATILE_SORT_OUTRO_SORT_OUTRO_SORT_H_
