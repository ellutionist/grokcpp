#ifndef GROKCPP_GROK_H
#define GROKCPP_GROK_H

#include "boost/regex.hpp"
#include "common.h"
#include "grok_match.h"
#include "grok_unit.h"
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace grok {
class Grok : std::enable_shared_from_this<Grok> {
public:
  explicit Grok(const std::string &grok_expr);

  Grok(const Grok &) = default;
  Grok(Grok &&) = default;

  int match(const std::string &str, GrokMatch &match) const;

  Grok &operator=(const Grok &) = default;

public:
  void register_self(const std::string &name) const;
  void register_self_force(const std::string &name) const;
  static void register_patterns(const string_pair_vector &);
  static int register_patterns_from_text(const std::string &text);
  static int register_patterns_from_file(const std::string &file_path);
  static int register_patterns_from_dir(const std::string &dir_path);
  static void reset_register() { register_groks_.clear(); }

private:
  boost::regex regex_;
  // TODO: add unit test for capture_identifiers_
  std::shared_ptr<string_vector> capture_identifiers_{new string_vector()};
  std::string grok_expr_;
  std::string raw_regex_expr_;

  Grok(const std::string &grok_expr, const std::vector<GrokUnit> &units);

  void init(const std::vector<GrokUnit> &units);

  static std::mutex register_lock_;

  static std::unordered_map<std::string, Grok> register_groks_;

  static int
  check_unregister_groks(const std::vector<GrokUnit> to_check,
                         std::shared_ptr<std::vector<GrokUnit>> container);

  static int __register_predefine_status;
};

inline std::unordered_map<std::string, Grok> Grok::register_groks_;

} // namespace grok
#endif
