#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <random>
#include <vector>

// A vector is said to be small if it contains these many or fewer elements.
size_t constexpr chunk_size = 15;

// Function prototypes are required for mutually recursive functions.
template <typename Type> Type quickselect(std::vector<Type>& vec, size_t pos, bool recursive=false);
template <typename Type> Type get_pivot(std::vector<Type> const& vec);

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
/// Choose a good pivot for partitioning the vector. This function should not
/// be called with small vectors.
///////////////////////////////////////////////////////////////////////////////
template <typename Type>
Type get_pivot(std::vector<Type> const& vec)
{
    // Find the median of each full-sized chunk of the vector.
    std::vector<Type> medians(vec.size() / chunk_size);
    for(size_t i = 0, j = 0; i + chunk_size - 1 < vec.size(); i += chunk_size, ++j)
    {
        std::vector<Type> chunk(&vec[i], &vec[i + chunk_size]);
        std::sort(chunk.begin(), chunk.end());
        medians[j] = chunk[chunk_size / 2];
    }

    return quickselect(medians, medians.size() / 2, true);
}

///////////////////////////////////////////////////////////////////////////////
/// Find the element which would be at the given position in the vector if the
/// vector were sorted. If the vector is small, the element is found by
/// actually sorting it.
///////////////////////////////////////////////////////////////////////////////
template <typename Type>
Type quickselect(std::vector<Type>& vec, size_t pos, bool recursive)
{
    size_t vec_size = vec.size();
    if(vec_size <= chunk_size)
    {
        // It is okay to mutate the input vector in a recursive call.
        // Otherwise, it came from the original caller, and must not be
        // modified.
        if(recursive)
        {
            std::sort(vec.begin(), vec.end());
            return vec[pos];
        }
        std::vector<Type> vec_copy(vec);
        std::sort(vec_copy.begin(), vec_copy.end());
        return vec_copy[pos];
    }

    // Divide the elements into three categories: less than, greater than and
    // equal to the pivot. There is no need to create a vector for the latter.
    Type pivot = get_pivot(vec);
    std::vector<Type> lows, highs;
    lows.reserve(vec_size);
    highs.reserve(vec_size);
    size_t pivots_size = 0;
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
            ++pivots_size;
        }
    }

    if(pos < lows.size())
    {
        return quickselect(lows, pos, true);
    }
    if(pos < lows.size() + pivots_size)
    {
        return pivot;
    }
    return quickselect(highs, pos - lows.size() - pivots_size, true);
}

///////////////////////////////////////////////////////////////////////////////
/// Test the implementation.
///////////////////////////////////////////////////////////////////////////////
void test_quickselect(int num_of_sizes)
{
    std::random_device device;
    std::mt19937 mersenne(device());
    std::uniform_int_distribution<int> distribution(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    auto generator = [&distribution, &mersenne](){ return distribution(mersenne); };
    for(int vec_size = 1; vec_size <= num_of_sizes; ++vec_size)
    {
        std::vector<int> vec(vec_size);
        std::generate(vec.begin(), vec.end(), generator);
        std::vector<int> vec_copy(vec);
        auto naive = naive_median_sandboxed(vec);

        auto start = std::chrono::steady_clock::now();
        auto efficient = quickselect(vec, vec.size() / 2);
        auto stop = std::chrono::steady_clock::now();
        if(vec != vec_copy)
        {
            throw std::runtime_error("Input vector was modified!");
        }
        if(naive != efficient)
        {
            throw std::runtime_error("Wrong result obtained!");
        }

        auto delay = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
        std::cout << std::setw(8) << vec_size << " ";
        std::cout << std::setw(12) << delay << "\n";
    }
}

///////////////////////////////////////////////////////////////////////////////
/// Main function.
///////////////////////////////////////////////////////////////////////////////
int main(void)
{
    test_quickselect(10000);
    return 0;
}
