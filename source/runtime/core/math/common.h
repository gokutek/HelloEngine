#pragma once

/**
 * @brief 
 * @tparam T 
 * @param value 
 * @param mask 
 * @return 
 */
template<typename T>
inline T AlignUpWithMask(T value, size_t mask)
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
inline T AlignDownWithMask(T value, size_t mask)
{
    return (T)((size_t)value & ~mask);
}

/**
 * @brief 
 * @tparam T 
 * @param value 
 * @param alignment 
 * @return 
 */
template<typename T>
inline T AlignUp(T value, size_t alignment)
{
    return AlignUpWithMask(value, alignment - 1);
}
