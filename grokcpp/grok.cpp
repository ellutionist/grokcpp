#include "grok.h"
#include "common.h"
#include <mutex>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace grok {

std::mutex Grok::register_lock_;

static const char *predefined_patterns_text =
#include "grok_patterns.txt"
    ;

int Grok::__register_predefine_status =
    Grok::register_patterns_from_text(predefined_patterns_text);

Grok::Grok(const std::string &grok_expr)
    : grok_expr_(grok_expr), raw_regex_expr_(grok_expr) {
  std::vector<GrokUnit> units = GrokUnit::scan_units(grok_expr);

  init(units);
}

Grok::Grok(const std::string &grok_expr, const std::vector<GrokUnit> &units)
    : grok_expr_(grok_expr), raw_regex_expr_(grok_expr) {

  init(units);
}

void Grok::init(const std::vector<GrokUnit> &units) {
  std::unordered_map<std::string, int> seen_identifiers;
  auto is_seen = [&seen_identifiers](const std::string &identifier) -> bool {
    return contains(seen_identifiers, identifier);
  };

  auto mark_seen = [&seen_identifiers](const std::string &identifier) {
    seen_identifiers.insert({identifier, 1});
  };

  for (const GrokUnit &unit : units) {
    const std::string &pattern = unit.pattern();
    const std::string &identifier = unit.identifier();

    if (!contains(register_groks_, pattern)) {
      throw std::runtime_error(
          string_format("unknown grok pattern \"%s\"", pattern.c_str()));
    }
    const auto &pre_g = register_groks_.at(pattern);

    bool is_new_identifier = identifier.size() && !is_seen(identifier);

    std::string interpreted =
        is_new_identifier ? string_format("(?<%s>%s)", identifier.c_str(),
                                          pre_g.raw_regex_expr_.c_str())
                          : pre_g.raw_regex_expr_;

    replace_all(raw_regex_expr_, unit.expr(), interpreted);

    if (is_new_identifier) {
      capture_identifiers_->emplace_back(identifier);

      mark_seen(identifier);
    }
  }

  regex_ = boost::regex(raw_regex_expr_);
}

int Grok::check_unregister_groks(
    const std::vector<GrokUnit> to_check,
    std::shared_ptr<std::vector<GrokUnit>> container) {

  int count = 0;
  for (const auto &unit : to_check) {
    const std::string &pattern = unit.pattern();

    if (!contains(register_groks_, pattern)) {
      ++count;
      if (container != nullptr) {
        container->emplace_back(unit);
      }
    }
  }
  return count;
}

void Grok::register_self(const std::string &name) const {
  if (contains(register_groks_, name)) {
    return;
  }
  std::lock_guard<std::mutex> guard(register_lock_);

  register_groks_.insert({name, *this});
  // register_groks_[name] = *this;
}

void Grok::register_self_force(const std::string &name) const {
  std::lock_guard<std::mutex> guard(register_lock_);

  decltype(register_groks_)::iterator it = register_groks_.find(name);
  if (it != register_groks_.end()) {
    it->second = *this;
  } else {
    register_groks_.insert({name, *this});
  }
}

int Grok::match(const std::string &str, GrokMatch &match) const {
  boost::smatch what;

  int re_success = boost::regex_search(str, what, this->regex_);
  if (re_success) {
    string_map values = std::unordered_map<std::string, std::string>();

    for (const auto &identifier : *capture_identifiers_) {
      std::string value = what[identifier];
      values.insert({identifier, value});
    }

    match = GrokMatch(this->capture_identifiers_, std::move(values));
    return 1;
  } else {
    return 0;
  }
}

} // namespace grok
