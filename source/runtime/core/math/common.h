#pragma once

/**
 * @brief 
 * @tparam T 
 * @param value 
 * @param mask 
 * @return 
 */
template<typename T>
inline T align_up_with_mask(T value, size_t mask)
{
    return (T)(((size_t)value + mask) & ~mask);
}

/**
 * @brief 
 * @tparam T 
 * @param value 
 * @param mask 
 * @return 
 */
template<typename T>
inline T align_down_with_mask(T value, size_t mask)
{
    return (T)((size_t)value & ~mask);
}

/**
 * @brief 向上对齐
 * @tparam T 
 * @param value 
 * @param alignment 
 * @return 
 */
template<typename T>
inline T align_up(T value, size_t alignment)
{
    return align_up_with_mask(value, alignment - 1);
}

/**
 * @brief 等价于ceil(1.0f*value/alignment)
 * @tparam T 
 * @param value 
 * @param alignment 
 * @return 
 */
template <typename T>
inline T divide_by_multiple(T value, size_t alignment)
{
    return (T)((value + alignment - 1) / alignment);
}
