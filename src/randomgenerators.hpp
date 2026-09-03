#pragma once
#include <iostream>
#include <random>
#include <limits>
#include <stdexcept>

inline size_t get_random_size_t(
    size_t min = 0, 
    size_t max = std::numeric_limits<size_t>::max()
) {
    // thread_local ensures these are only seeded and created once per thread
    thread_local std::random_device rd;
    thread_local std::mt19937_64 gen(rd());
    
    // The distribution is lightweight and can be created on every call
    std::uniform_int_distribution<size_t> distrib(min, max);
    
    return distrib(gen);
}