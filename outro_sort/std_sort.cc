#include <algorithm>

extern "C"
{
    void std_sort(int *begin, int *end)
    {
        std::sort(begin, end);
    }
}
