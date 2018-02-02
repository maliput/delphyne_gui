// Copyright 2018 Open Source Robotics Foundation
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "visualizer/GlobalAttributes.hh"

#include <stdexcept>

#include "gtest/gtest.h"

namespace delphyne {
namespace gui {
namespace global_attributes{
namespace test{

//////////////////////////////////////////////////

/// \brief Checks GlobalAttributes API to parse and query CLI strings.
TEST(GlobalAttributes, SampleUsage) {
  EXPECT_FALSE(HasArgument("foo"));

  const char* arguments[] = {
    "--foo=bar",
    "--bar=foo",
    "--sample_param=123param"
  };
  EXPECT_NO_THROW(ParseArguments(3, arguments));

  EXPECT_TRUE(HasArgument("foo"));
  EXPECT_TRUE(HasArgument("bar"));
  EXPECT_TRUE(HasArgument("sample_param"));
  EXPECT_EQ(GetArgument("foo"), "bar");
  EXPECT_EQ(GetArgument("bar"), "foo");
  EXPECT_EQ(GetArgument("sample_param"), "123param");
  EXPECT_FALSE(HasArgument("missing_argument"));
  EXPECT_THROW(GetArgument("missing_argument"), std::runtime_error);
}

/// \brief Checks GlobalAttributes possible conditions where methods should
/// throw.
TEST(GlobalAttributes, WrongParameters) {
  const char* wrong_arguments[] = {
    "--=", // Short string.
    "--b=", // Short string.
    "--foobar", // Missing "=".
    "foo=bar", // Missing "--".
    "--foo= bar", // Whitespace.
    "--foo=\tbar", // Tabulation.
    "--foo=\nbar", // Newline.
    "--foo=\rbar", // Carry return.
    "--=bar", // Missing key.
    "--foo=", // Missing value.
  };
  for (int i = 0; i < 9; ++i) {
    EXPECT_THROW(ParseArguments(1, &(wrong_arguments[i])), std::runtime_error);
  }
}

//////////////////////////////////////////////////
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}
}
}
}
