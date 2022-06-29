#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits>
#include <random>
#include <vector>

template <typename Type>
Type quickselect(std::vector<Type> const& vec, size_t pos);

///////////////////////////////////////////////////////////////////////////////
/// Display a vector.
///////////////////////////////////////////////////////////////////////////////
template <typename Type>
void print_vector(std::vector<Type> const& vec)
{
    std::cout << "[";
    for(auto const& v: vec)
    {
        std::cout << v << ", ";
    }
    std::cout << "]\n";
}

///////////////////////////////////////////////////////////////////////////////
/// Find the median of a vector by sorting it. This mutates the input vector!
///////////////////////////////////////////////////////////////////////////////
template <typename Type>
Type naive_median(std::vector<Type>& vec)
{
    std::sort(vec.begin(), vec.end());
    return vec[vec.size() / 2];
}

///////////////////////////////////////////////////////////////////////////////
/// Find the median of a vector by sorting it.
///////////////////////////////////////////////////////////////////////////////
template <typename Type>
Type naive_median_sandboxed(std::vector<Type> vec)
{
    std::sort(vec.begin(), vec.end());
    return vec[vec.size() / 2];
}

///////////////////////////////////////////////////////////////////////////////
/// Pick a good pivot from the elements of the given vector.
///////////////////////////////////////////////////////////////////////////////
template <typename Type>
Type get_pivot(std::vector<Type> const& vec)
{
    size_t constexpr chunk_size = 5;
    if(vec.size() <= chunk_size)
    {
        return naive_median_sandboxed(vec);
    }

    // Find the median of each full-sized chunk of the vector.
    std::vector<Type> medians(vec.size() / chunk_size);
    for(size_t i = 0, j = 0; i + 4 < vec.size(); i += 5, ++j)
    {
        std::vector<Type> chunk(&vec[i], &vec[i + 5]);
        medians[j] = naive_median(chunk);
    }

    return quickselect(medians, medians.size() / 2);
}

///////////////////////////////////////////////////////////////////////////////
/// Find the element which would be at the given position in the vector if the
/// vector were sorted.
///////////////////////////////////////////////////////////////////////////////
template <typename Type>
Type quickselect(std::vector<Type> const& vec, size_t pos)
{
    if(vec.size() == 1)
    {
        return vec[0];
    }

    // Divide the elements into three categories.
    Type const pivot = get_pivot(vec);
    std::vector<Type> lows, pivots, highs;
    for(auto const& v: vec)
    {
        if(v < pivot)
        {
            lows.push_back(v);
        }
        else if(v > pivot)
        {
            highs.push_back(v);
        }
        else
        {
            pivots.push_back(v);
        }
    }

    if(pos < lows.size())
    {
        return quickselect(lows, pos);
    }
    if(pos < lows.size() + pivots.size())
    {
        return pivots[0];
    }
    return quickselect(highs, pos - lows.size() - pivots.size());
}

///////////////////////////////////////////////////////////////////////////////
/// Test the implementation.
///////////////////////////////////////////////////////////////////////////////
void test_quickselect(void)
{
    std::random_device device;
    std::mt19937 mersenne(device());
    std::uniform_int_distribution<int> distribution(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    auto generator = [&distribution, &mersenne](){ return distribution(mersenne); };
    for(int vec_size = 1; vec_size <= 1000; ++vec_size)
    {
        std::vector<int> vec(vec_size);
        std::generate(vec.begin(), vec.end(), generator);
        auto naive = naive_median_sandboxed(vec);
        auto efficient = quickselect(vec, vec.size() / 2);
        if(naive != efficient)
        {
            throw std::runtime_error("Wrong result obtained!");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// Main function.
///////////////////////////////////////////////////////////////////////////////
int main(void)
{
    test_quickselect();
    return 0;
}
