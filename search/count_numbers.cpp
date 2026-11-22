// Take a sorted vector of numbers, and efficiently find the count of all numbers that are less
// than a given parameter - given as *lessThan* (c below).

// Idea: find the transition point d such that v[d-1] < c <= v[d].
// Everything before d is less than c, so the answer is d.
// Keep an interval [a,b] of indices with v[a] < c <= v[b], so d lies in (a,b]. With mid = (a+b)/2:
// v[mid] < c  : we can restrict our attention to [mid, b]
// v[mid] >= c : we can restrict our attention to [a, mid]
// The interval shrinks until b = a+1, and then d = b.
// Start with [0, size-1], after returning early when c <= v[0] (answer 0) or v[size-1] < c (answer size).

#include <vector>
#include <stdexcept>
#include <iostream>

int countNumbers(const std::vector<int>& sortedVector, int lessThan)
{
    if (sortedVector.empty() || sortedVector.front() >= lessThan) {
        return 0;
    }

    if (sortedVector.back() < lessThan) {
        return static_cast<int>(sortedVector.size());
    }

    std::size_t a = 0;
    std::size_t b = sortedVector.size() - 1;

    while (b - a > 1) {
        const std::size_t mid = (a + b) / 2;

        if (sortedVector[mid] < lessThan) {
            a = mid;
        } else {
            b = mid;
        }
    }

    return static_cast<int>(b);
}

#ifndef RunTests
int main()
{
    std::vector<int> v { 1, 2, 4, 7, 9 };
    std::cout << countNumbers(v, 4); // 2
}
#endif
