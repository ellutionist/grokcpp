#ifndef GROKCPP_GROKUNIT_H
#define GROKCPP_GROKUNIT_H

#include "boost/regex.hpp"
#include "common.h"
#include <vector>

namespace grok {
class GrokUnit {
private:
  std::string pattern_;
  std::string identifier_;

  std::string expr_;

  // static boost::regex __grok_unit_regex;

  static boost::regex grok_unit_regex() {
    static const boost::regex __unit_regex{boost::regex(
        R"(%\{(?<PATTERN>[A-Z0-9_]+)(?::(?<IDENTIFIER>[a-z0-9_]+))?\})")};
    return __unit_regex;
  }

public:
  GrokUnit(const std::string &pattern, const std::string &identifier);

  GrokUnit(const std::string &pattern);

  // GrokUnit& operator=(const GrokUnit &) = default;
  // GrokUnit& operator=(GrokUnit &&) = default;

  inline const std::string &pattern() const { return pattern_; }

  inline const std::string &identifier() const { return identifier_; }

  inline const std::string &expr() const { return expr_; }

  static std::vector<GrokUnit> scan_units(const std::string &to_scan);

  bool operator==(const GrokUnit &other) const;
};
}; // namespace grok

#endif
