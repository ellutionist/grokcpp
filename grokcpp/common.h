#ifndef GROKCPP_COMMON_H
#define GROKCPP_COMMON_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "config.h"

namespace grok {

using string_vector = std::vector<std::string>;
using string_pair_vector = std::vector<std::pair<std::string, std::string>>;

template <typename... Args>
std::string string_format(const std::string &format, Args... args) {
  int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) +
               1; // Extra space for '\0'
  if (size_s <= 0) {
    throw std::runtime_error("Error during formatting.");
  }
  auto size = static_cast<size_t>(size_s);
  std::unique_ptr<char[]> buf(new char[size]);
  std::snprintf(buf.get(), size, format.c_str(), args...);
  return std::string(buf.get(),
                     buf.get() + size - 1); // We don't want the '\0' inside
}

template <typename T, typename V>
bool contains(const std::unordered_map<T, V> m, const T &key) {
  return m.find(key) != m.end();
}

inline void replace_all(std::string &source, const std::string &target,
                        const std::string &sub) {
  size_t pos;

  while ((pos = source.find(target)) != std::string::npos) {
    source.replace(pos, target.length(), sub);
  }
}

inline std::vector<std::string> split(const std::string &s,
                                      const std::string &delimiter, int n = 0) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);

    if (n > 0 && res.size() >= n - 1) {
      break;
    }
  }

  res.push_back(s.substr(pos_start));
  return res;
}

#define __FILENAME__                                                           \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#if GROKCPP_DEBUG == 1
#define GROK_DEBUG_LOGF(fmt, ...)                                              \
  std::cout << grok::string_format(                                            \
                   "%s:%d: %s", __FILENAME__, __LINE__,                       \
                   grok::string_format(fmt, __VA_ARGS__).c_str())              \
            << std::endl;
#else
#define GROK_DEBUG_LOGF(fmt, ...) (void(0))
#endif

} // namespace grok

#endif // GROKCPP_COMMON_H
