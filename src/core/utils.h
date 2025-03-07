#pragma once

#include <string>
#include <charconv> // Required for std::from_chars

template <typename T>
static bool tryParseString(const std::string &str, T &result)
{
    if (str.size() == 0)
        return false;
    const char *first = str.data();
    const char *last = first + str.size();

    std::from_chars_result res = std::from_chars(first, last, result);

    if (res.ec == std::errc())
        return true;
    return false;
}