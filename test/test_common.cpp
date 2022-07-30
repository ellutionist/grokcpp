#include "common.h"
#include "gtest/gtest.h"
#include <string>
#include <unordered_map>

TEST(COMMON, FORMAT) {
  std::string s = grok::string_format(
      "a string: %s, an integer: %03d, a float: %.2f", "string", 25, 0.4321);

  EXPECT_EQ(s, "a string: string, an integer: 025, a float: 0.43");
}

TEST(COMMON, CONTAINS) {
  std::unordered_map<std::string, std::string> map;
  map.insert({"valid_key", "value"});

  EXPECT_EQ(grok::contains(map, std::string("valid_key")), true);
  EXPECT_EQ(grok::contains(map, std::string("invalid_key")), false);
}

TEST(COMMON, SPLIT) {
  auto parts = grok::split("one;two;three", ";");
  EXPECT_EQ(parts, decltype(parts)({"one", "two", "three"}));

  parts = grok::split("how are you", " ", 2);
  EXPECT_EQ(parts, decltype(parts)({"how", "are you"}));
}

TEST(COMMON, FILE_NAME_MACRO) { EXPECT_EQ(__FILE_NAME__, "test_common.cpp"); }
