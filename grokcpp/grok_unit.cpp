#include "grok_unit.h"
#include "boost/regex.hpp"
#include <cassert>
#include <vector>

namespace grok {
// auto GrokUnit::__grok_unit_regex{boost::regex(
// R"(%\{(?<PATTERN>[A-Z0-9_]+)(?::(?<IDENTIFIER>[a-z0-9_]+))?\})")};

GrokUnit::GrokUnit(const std::string &pattern, const std::string &identifier)
    : pattern_(pattern), identifier_(identifier) {
  assert(identifier_.size());
  assert(pattern_.size());
  expr_ = string_format(R"(%%{%s:%s})", pattern.c_str(), identifier.c_str());
}
GrokUnit::GrokUnit(const std::string &pattern) : pattern_(pattern) {
  assert(pattern_.size());
  expr_ = string_format(R"(%%{%s})", pattern.c_str());
}

std::vector<GrokUnit> GrokUnit::scan_units(const std::string &to_scan) {
  std::vector<GrokUnit> units;

  boost::sregex_iterator iter =
      boost::make_regex_iterator(to_scan, grok_unit_regex());
  boost::sregex_iterator iter_end;

  for (; iter != iter_end; ++iter) {
    boost::smatch s = *iter;

    std::string pattern = s["PATTERN"];
    std::string identifier = s["IDENTIFIER"];

    if (identifier.empty()) {
      units.emplace_back(GrokUnit(pattern));
    } else {
      units.emplace_back(GrokUnit(pattern, identifier));
    }
  }

  return units;
}

bool GrokUnit::operator==(const GrokUnit &other) const {
  return other.pattern_ == this->pattern_ &&
         other.identifier_ == this->identifier_;
};

} // namespace grok
