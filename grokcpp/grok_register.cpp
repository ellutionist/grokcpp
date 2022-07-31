#include "common.h"
#include "grok.h"
#include <algorithm>
#include <exception>
#include <sstream>
#include <stdexcept>
#include <sys/dirent.h>
#include <unordered_map>
#include <utility>
#include <vector>

namespace grok {

// std::vector<std::string> split(const std::string &s,
// const std::string &delimiter) {
// size_t pos_start = 0, pos_end, delim_len = delimiter.length();
// std::string token;
// std::vector<std::string> res;

// while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
// token = s.substr(pos_start, pos_end - pos_start);
// pos_start = pos_end + delim_len;
// res.push_back(token);
//}

// res.push_back(s.substr(pos_start));
// return res;
//}

struct pattern_expression_t {
  std::string pattern;
  std::string expr;
  std::vector<GrokUnit> units;

  pattern_expression_t *pre = nullptr;
  pattern_expression_t *next = nullptr;

  pattern_expression_t(){};
  pattern_expression_t(const std::string &__pattern, const std::string &__expr)
      : pattern(__pattern), expr(__expr), units() {}

  pattern_expression_t(const std::string &__pattern, const std::string &__expr,
                       std::vector<GrokUnit> &&__units)
      : pattern(__pattern), expr(__expr), units(__units) {}
};

template <typename T> class ListGuard {
private:
  T *head_;

public:
  ListGuard(T *head) : head_(head){};
  ~ListGuard() {
    T *current = head_;

    while (current != nullptr) {
      T *pre = current;
      current = current->next;
      delete pre;
    }
  }
};

string_pair_vector preprocess_patterns_text(const std::string &text) {
  string_pair_vector result;
  for (const std::string &line : split(text, "\n")) {
    if (line.find_first_of('#') == line.find_first_not_of(' ')) {
      // even works for empty string
      continue;
    }

    std::vector<std::string> pair = split(line, " ", 2);
    if (pair.size() < 2) {
      throw std::runtime_error(
          string_format("invalid pattern line: %s", line.c_str()));
    }
    result.emplace_back(std::make_pair(pair[0], pair[1]));

    GROK_DEBUG_LOGF("<line> %s", line.c_str());
    GROK_DEBUG_LOGF("\t<pair> %s || %s", pair[0].c_str(), pair[1].c_str());
  }

  return result;
}

pattern_expression_t *
get_list_from_pattern_expr(const string_pair_vector &pattern_expr_vec) {
  pattern_expression_t *head = new pattern_expression_t;
  pattern_expression_t *current = head;

  std::unordered_map<std::string, int> seen_patterns;

  for (const auto &pair : pattern_expr_vec) {
    const std::string &pattern = pair.first;
    const std::string &expr = pair.second;

    if (contains(seen_patterns, pattern)) {
      throw std::runtime_error(
          string_format("duplicate pattern \"%s\" (expr: \"%s\")",
                        pattern.c_str(), expr.c_str()));
    }
    seen_patterns[pattern] = 1;

    auto units = GrokUnit::scan_units(expr);
    pattern_expression_t *node =
        new pattern_expression_t(pattern, expr, std::move(units));

    current->next = node;
    node->pre = current;
    current = node;
  }

  return head;
}

void Grok::register_patterns(const string_pair_vector &pattern_expr_vec) {
  pattern_expression_t *head = get_list_from_pattern_expr(pattern_expr_vec);
  ListGuard<pattern_expression_t> guard(head);

  for (;;) {
    int progress_count = 0;
    pattern_expression_t *current = head->next;

    while (current != nullptr) {
      pattern_expression_t *next = current->next;
      int unknown_count = Grok::check_unregister_groks(current->units, nullptr);
      if (!unknown_count) {
        GROK_DEBUG_LOGF("%s || %s", current->pattern.c_str(),
                        current->expr.c_str());
        Grok g{current->expr, current->units};
        g.register_self(current->pattern);
        ++progress_count;

        current->pre->next = next;
        if (next != nullptr) {
          next->pre = current->pre;
        }
        delete current;
      }
      current = next;
    }

    if (head->next == nullptr) {
      break;
    }

    if (progress_count == 0) {
      int remain_pattern_count = 0;

      std::stringstream ss;
      for (pattern_expression_t *node = head; node != nullptr;
           node = node->next) {
        ++remain_pattern_count;

        if (remain_pattern_count)
          ss << ", ";
        ss << node->pattern;
      }
      throw std::runtime_error(string_format(
          "there are %d patterns (%s) relying on unknown patterns.",
          remain_pattern_count, ss.str().c_str()));
    }
  }
}

int Grok::register_patterns_from_text(const std::string &text) {
  string_pair_vector pattern_expressions = preprocess_patterns_text(text);
  register_patterns(pattern_expressions);
  return 1;
}

int Grok::register_patterns_from_file(const std::string &file_path) {
  std::string text = read_file(file_path);

  return register_patterns_from_text(text);
}

int Grok::register_patterns_from_dir(const std::string &dir_path) {
  auto file_paths = list_dir(dir_path);

  std::for_each(file_paths.begin(), file_paths.end(),
                [](const std::string &file_path) {
                  register_patterns_from_file(file_path);
                });

  return 0;
}

//}
} // namespace grok
