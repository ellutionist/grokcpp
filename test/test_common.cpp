#include "common.h"
#include "gtest/gtest.h"

TEST(COMMON, SPLIT) {
  auto parts = grok::split("one;two;three", ";");
  EXPECT_EQ(parts, decltype(parts)({"one", "two", "three"}));

  parts = grok::split("how are you", " ", 2);
  EXPECT_EQ(parts, decltype(parts)({"how", "are you"}));
}
