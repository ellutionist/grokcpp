#include "grok.h"
#include "gtest/gtest.h"
#include <algorithm>
#include <utility>
#include <vector>

using namespace grok;

struct test_cast_t {
  std::string grok_expr;
  std::string target;
  string_pair_vector expected_key_value_pairs;

  void run_test_case() const {
    Grok g(this->grok_expr);
    GrokMatch match;
    EXPECT_EQ(g.match(this->target, match), 1);

    for (const std::pair<std::string, std::string> &p :
         this->expected_key_value_pairs) {
      EXPECT_EQ(match[p.first], p.second);
    }
  }
};
TEST(GROK_PREDEFINE, PREDEFINE) {

  std::vector<test_cast_t> test_cases = {
      {
          "username=%{USERNAME:myusername}",
          "username=myusername3055",
          {
              {"myusername", "myusername3055"},
          },
      },
      {
          "username=\"%{USER:new_user_name}\"",
          "a useless sentence. now username=\"newusername2144\"...",
          {
              {"new_user_name", "newusername2144"},
          },
      },
      {
          "email address: %{EMAILADDRESS:email}",
          "<<<email address: impc53802956@gmail.com<<<",
          {
              {"email", "impc53802956@gmail.com"},
          },
      },
      {
          "%{UUID:uuid}",
          "id2497b0a2-0076-4ce5-89f8-4149762d3db5",
          {
              {"uuid", "2497b0a2-0076-4ce5-89f8-4149762d3db5"},
          },
      },
      {
          R"(%{MAC:first}\s+%{MAC:second}\s+%{MAC:third})",
          "82:66:b0:89:c8:01   aa-c3-a8-8f-0a-fa   b1ec.a205.fca2",
          {
              {"first", "82:66:b0:89:c8:01"},
              {"second", "aa-c3-a8-8f-0a-fa"},
              {"third", "b1ec.a205.fca2"},
          },
      },
      {
          R"(%{URIPROTO:protocol}://%{URIHOST:host}%{URIPATH:path}%{URIPARAM:param})",
          "https://grok.com/grokcpp?version=1",
          {
              {"protocol", "https"},
              {"host", "grok.com"},
              {"path", "/grokcpp"},
              {"param", "?version=1"},
          },
      },

  };

  for (const test_cast_t &c : test_cases) {
    c.run_test_case();
  }
}
