// Copyright 2017 Toyota Research Institute

#include "visualizer/global_attributes.hh"

#include <stdexcept>

#include "gtest/gtest.h"

namespace delphyne {
namespace gui {
namespace test {

//////////////////////////////////////////////////

/// \brief Checks GlobalAttributes API to parse and query CLI strings.
TEST(GlobalAttributes, SampleUsage) {
  EXPECT_FALSE(GlobalAttributes::HasArgument("foo"));

  const char* arguments[] = {"--foo=bar", "--bar=foo", "--sample_param=123param"};
  EXPECT_NO_THROW(GlobalAttributes::ParseArguments(3, arguments));

  EXPECT_TRUE(GlobalAttributes::HasArgument("foo"));
  EXPECT_TRUE(GlobalAttributes::HasArgument("bar"));
  EXPECT_TRUE(GlobalAttributes::HasArgument("sample_param"));
  EXPECT_EQ("bar", GlobalAttributes::GetArgument("foo"));
  EXPECT_EQ("foo", GlobalAttributes::GetArgument("bar"));
  EXPECT_EQ("123param", GlobalAttributes::GetArgument("sample_param"));
  EXPECT_FALSE(GlobalAttributes::HasArgument("missing_argument"));
  EXPECT_THROW(GlobalAttributes::GetArgument("missing_argument"), std::runtime_error);
}

/// \brief Checks GlobalAttributes possible conditions where methods should
/// throw.
TEST(GlobalAttributes, WrongParameters) {
  const char* wrong_arguments[] = {
      "--=",          // Short string.
      "--b=",         // Short string.
      "--foobar",     // Missing "=".
      "foo=bar",      // Missing "--".
      "--foo= bar",   // Whitespace.
      "--foo=\tbar",  // Tabulation.
      "--foo=\nbar",  // Newline.
      "--foo=\rbar",  // Carry return.
      "--=bar",       // Missing key.
      "--foo=",       // Missing value.
  };
  for (int i = 0; i < 9; ++i) {
    EXPECT_THROW(GlobalAttributes::ParseArguments(1, &(wrong_arguments[i])), std::runtime_error);
  }
}

//////////////////////////////////////////////////
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}  // namespace test
}  // namespace gui
}  // namespace delphyne
