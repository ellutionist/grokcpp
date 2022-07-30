#ifndef GROKCPP_GROKMATCH_H
#define GROKCPP_GROKMATCH_H

#include "common.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace grok {
class Grok;

using string_map = std::unordered_map<std::string, std::string>;
class GrokMatch {
  friend Grok;

private:
  std::shared_ptr<string_vector> capture_identifiers_{nullptr};
  string_map values_{};

public:
  GrokMatch() {}
  GrokMatch(std::shared_ptr<string_vector> capture_identifiers,
            string_map &&values)
      : capture_identifiers_(capture_identifiers), values_(values) {}

  std::string get(const std::string &identifier) const {
    if (contains(values_, identifier)) {
      return values_.at(identifier);
    } else {
      return "";
    }
  }

  std::string operator[](const std::string &key) const { return get(key); }

  // TODO: implement iterator here
};
}; // namespace grok
#endif
