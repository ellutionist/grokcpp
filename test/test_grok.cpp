#include "common.h"
#include "grok.h"
#include "gtest/gtest.h"
#include <cstdio>
#include <fstream>
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

TEST(GROK, REGISTER_FORCE) {
  grok::Grok::reset_register();
  {
    grok::Grok g{"[A-Z]+"};
    g.register_self_force("CAPS");
  }

  {
    grok::Grok g{"%{CAPS:word}"};
    grok::GrokMatch match;
    g.match("abc, ABC", match);

    EXPECT_EQ(match["word"], "ABC");
  }

  {
    grok::Grok g{"[a-z]+"};
    g.register_self("CAPS");
  }

  {
    grok::Grok g{"%{CAPS:word}"};
    grok::GrokMatch match;
    g.match("abc, ABC", match);

    EXPECT_NE(match["word"], "abc");
  }

  {
    grok::Grok g{"[a-z]+"};
    g.register_self_force("CAPS");
  }

  {
    grok::Grok g{"%{CAPS:word}"};
    grok::GrokMatch match;
    g.match("abc, ABC", match);

    EXPECT_EQ(match["word"], "abc");
  }
}

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

TEST(GROK, REGISTER_FILE) {
  grok::Grok::reset_register();

  std::string tmp_path =
      grok::string_format("/tmp/%s", grok::gen_random(12).c_str());
  {
    std::ofstream tmp_output;
    tmp_output.open(tmp_path);
    tmp_output << "NUMBERS [0-9]+" << '\n';
    tmp_output << "LOWERS [a-z]+" << '\n';
    tmp_output.close();
  }

  grok::Grok::register_patterns_from_file(tmp_path);

  grok::Grok g {"name: %{LOWERS:myname}, age: %{NUMBERS:myage}"};
  grok::GrokMatch match;

  g.match("name: mike, age: 15", match);
  EXPECT_EQ(match["myname"], "mike");
  EXPECT_EQ(match["myage"], "15");

  std::remove(tmp_path.c_str());
}

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
