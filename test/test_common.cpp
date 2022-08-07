#include "common.h"
#include "gtest/gtest.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
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

TEST(COMMON, READ_FILE) {
  namespace fs = std::filesystem;
  auto tmp_dir = fs::temp_directory_path();
  auto tmp_file_name = fs::path(grok::gen_random(12));
  auto tmp_file_path = tmp_dir / tmp_file_name;

  std::ofstream tmp_file_out;
  tmp_file_out.open(tmp_file_path);
  std::stringstream ss;

  for (int i = 0; i < 128; ++i) {
    std::string random_string = grok::gen_random(1024);
    ss << random_string << "\n";

    tmp_file_out << random_string << "\n";
  }

  tmp_file_out.close();

  auto read_text = grok::read_file(tmp_file_path);

  fs::remove(tmp_file_path);

  EXPECT_EQ(ss.str(), read_text);
}

TEST(COMMON, FILE_NAME_MACRO) {
  EXPECT_EQ(std::string(__FILENAME__), std::string("test_common.cpp"));
}
