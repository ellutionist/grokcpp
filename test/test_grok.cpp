#include "grok.h"
#include "gtest/gtest.h"
#include <ostream>

TEST(GROK, REGISTER_BASE) {
  grok::Grok::reset_register();
  grok::string_pair_vector vec{
      {"WORD_NUMBERS", "%{NUMBERS} %{WORD}"},
      {"NUMBERS", "[0-9]+"},
      {"WORD_NUMBERS_REPEAT",
       "%{NUMBERS:anum}%{NUMBERS} %{WORD} %{WORD:aword}"},
      {"WORD", "[a-zA-Z]+"},
  };

  grok::Grok::register_patterns(vec);
  grok::Grok g{"%{WORD}____%{NUMBERS}===%{WORD_NUMBERS_REPEAT:some_part}"};
};

TEST(GROK, REGISTER_FAIL) {
  grok::Grok::reset_register();
  grok::string_pair_vector vec{
      {"WORD_NUMBERS", "%{NUMBERS} %{WORD}"},
      //{"NUMBERS", "[0-9]+"},
      {"WORD_NUMBERS_REPEAT",
       "%{NUMBERS:anum}%{NUMBERS} %{WORD} %{WORD:aword}"},
      {"WORD", "[a-zA-Z]+"},
  };

  EXPECT_THROW(grok::Grok::register_patterns(vec);, std::runtime_error);
};

TEST(GROK, MATCH_BASE) {
  grok::Grok::reset_register();
  grok::string_pair_vector vec{
      {"WORD_NUMBERS", "%{WORD}_%{NUMBERS}"},
      {"NUMBERS", "[0-9]+"},
      {"WORD_NUMBERS_REPEAT",
       "%{NUMBERS:anum}%{NUMBERS} %{WORD} %{WORD:aword}"},
      {"WORD", "[a-zA-Z]+"},
  };

  grok::Grok::register_patterns(vec);

  {
    grok::Grok g{"name: \"%{WORD:name}\"; age: %{NUMBERS:age}; id: "
                 "\"%{WORD_NUMBERS:id}\""};
    std::string s{R"(name: "steve"; age: 23; id: "killer_001")"};

    grok::GrokMatch match;

    int status = g.match(s, match);
    EXPECT_EQ(status, 1);
    EXPECT_EQ(match["name"], "steve");
    EXPECT_EQ(match.get("age"), "23");
    EXPECT_EQ(match["id"], "killer_001");
    EXPECT_EQ(match["unknown"], "");
  }

  {
    grok::Grok g{R"(no.%{NUMBERS:code}\s+name:\s*%{WORD:name}\s+%{WORD:name})"};
    std::string s{R"(no.3055    name:  cplusplus   grok)"};

    grok::GrokMatch match;
    int status = g.match(s, match);
    EXPECT_EQ(status, 1);
    EXPECT_EQ(match["code"], "3055");
    EXPECT_EQ(match["name"], "cplusplus");
    EXPECT_EQ(match["unknown"], "");
  }
}

//TEST(GROK, PREDEFINE) {
    //std::string pre (predefined_patterns_text);

    //ASSERT_GT(pre.size(), 0);
    //SUCCEED() << "predefined_patterns_text length: " << pre.size() << std::endl;
//}
