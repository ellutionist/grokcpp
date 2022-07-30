#include "grok_unit.h"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <vector>

TEST(GROK_UNIT, BASE) {
  grok::GrokUnit u{"NUMBER", "amount"};

  EXPECT_EQ(u.pattern(), "NUMBER");
  EXPECT_EQ(u.identifier(), "amount");
  EXPECT_EQ(u.expr(), "%{NUMBER:amount}");

  u = {"DATE", "birthday"};

  EXPECT_EQ(u.pattern(), "DATE");
  EXPECT_EQ(u.identifier(), "birthday");
  EXPECT_EQ(u.expr(), "%{DATE:birthday}");

  u = {"NOTEQUAL", "not_equal"};
  EXPECT_NE(u.expr(), "%{NOTEQUAL}");
  EXPECT_NE(u.expr(), "%{NOTEQUAL:equal}");

  u = {"DATETIME"};

  EXPECT_EQ(u.pattern(), "DATETIME");
  EXPECT_EQ(u.identifier(), "");
  EXPECT_EQ(u.expr(), "%{DATETIME}");
};

TEST(GROK_UNIT, SCAN_UNITS) {
  std::vector<grok::GrokUnit> expected = {{"URL", "url"},
                                          {"DATETIME", "datetime"},
                                          {"MONTHDAY"},
                                          {"ADDRESS", "residence"}};

  std::string s{"hello world. %{URL:url} ... "
                "%{DATETIME:datetime}====%{MONTHDAY}....MEANINGLESS%%%%%%{"
                "ADDRESS:residence}"};

  auto units = grok::GrokUnit::scan_units(s);

  EXPECT_EQ(expected, units);

  expected = {{"URL", "url"},
              {"DATETIME", "datetime"},
              {"MONTHDAY"},
              {"OUTLIER", "outlier"},
              {"ADDRESS", "residence"}};

  s = "hello world. %{URL:url} ... "
      "%{DATETIME:datetime}====%{MONTHDAY}....MEANINGLESS%{ADDRESS:residence}@"
      "@@%{OUTLIER:outlier}";

  units = grok::GrokUnit::scan_units(s);

  EXPECT_NE(expected, units);
}
