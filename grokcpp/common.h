#ifndef GROKCPP_COMMON_H
#define GROKCPP_COMMON_H

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <mutex>

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

inline std::string read_file(const std::string &file_path) {
  std::ifstream the_file(file_path);
  std::stringstream buffer;
  buffer << the_file.rdbuf();

  the_file.close();

  return buffer.str();
}

inline std::string gen_random(const int len) {
  static std::once_flag random_seed_once;
  static const char alphanum[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";
  std::string tmp_s;
  tmp_s.reserve(len);

  std::call_once(random_seed_once, std::srand, time(NULL));
  for (int i = 0; i < len; ++i) {
    tmp_s += alphanum[std::rand() % (sizeof(alphanum) - 1)];
  }

  return tmp_s;
}

inline string_vector list_dir(const std::string &dir_path) {
  namespace fs = std::filesystem;
  string_vector file_paths;
  for (const auto &file : fs::directory_iterator(dir_path)) {
    file_paths.emplace_back(file.path());
  }

  return file_paths;
}

#if __APPLE__ || defined(unix) || defined(__unix__) || defined(__unix)
#define DIR_SEPARATOR '/'
#else
#define DIR_SEPARATOR '\\'
#endif

#define __FILENAME__                                                           \
  (strrchr(__FILE__, DIR_SEPARATOR) ? strrchr(__FILE__, DIR_SEPARATOR) + 1     \
                                    : __FILE__)

#if GROKCPP_DEBUG == 1
#define GROK_DEBUG_LOGF(fmt, ...)                                              \
  std::cout << grok::string_format(                                            \
                   "%s:%d: %s", __FILENAME__, __LINE__,                        \
                   grok::string_format(fmt, __VA_ARGS__).c_str())              \
            << std::endl;
#else
#define GROK_DEBUG_LOGF(fmt, ...) (void(0))
#endif

} // namespace grok

#endif // GROKCPP_COMMON_H
