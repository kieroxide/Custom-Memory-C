#pragma once

#include <cstddef>

#define WINDOWS_FREE_ALL 0

inline constexpr size_t HEAP_ALIGNMENT = alignof(std::max_align_t);