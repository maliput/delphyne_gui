// BSD 3-Clause License
//
// Copyright (c) 2022, Woven Planet. All rights reserved.
// Copyright (c) 2017-2022, Toyota Research Institute. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef GLOBAL_ATTRIBUTES_HH
#define GLOBAL_ATTRIBUTES_HH

#include <map>
#include <string>

namespace delphyne {
namespace gui {

/// \brief Holds methods to parse and query CLI arguments.
///
/// \details Developers should not instantiate an object of this type but use
///          the API that this class provides.
class GlobalAttributes {
  /// \brief Parses a list of CLI arguments and holds them as pairs key-value.
  /// \details Each element of @p argv must be "--{KEY}={VALUE}". The key will
  /// be KEY, and the value will be VALUE. Note that "--" and "=" are necessary.
  /// Each element of @p argv should not have: white spaces, '\\n', '\\r', '\\t'
  /// characters and must be at least 5 characters long including "--" and "=".
  /// \param[in] argc The number of elements in @p argv.
  /// \param[in] argv A vector of string constants that hold the arguments.
  /// \throws std::runtime_error When any item of @p argv contains: white
  /// spaces, '\\n', '\\r' or '\\t' characters.
  /// \throws std::runtime_error When any item of @p argv does not start with
  /// "--".
  /// \throws std::runtime_error When any item of @p argv does not have "=" from
  /// the third position on.
  /// \throws std::runtime_error When key or value any item of @p argv are empty
  /// strings.
 public:
  static void ParseArguments(int argc, const char* const* argv);

  /// \brief Query the @p key existence.
  /// \param[in] key A string to index the value.
  /// \return true When @p key exists, false otherwise.
 public:
  static bool HasArgument(const std::string& key);

  /// \brief Gets a CLI argument previously set using SetArgument.
  /// \param[in] key A string to index the value.
  /// \return A std::string with the argument.
  /// \throws std::runtime_error When there is no value for @p key.
 public:
  static std::string GetArgument(const std::string& key);

  // \brief Map of strings to hold global attributes.
 private:
  static std::map<std::string, std::string> globalAttributes;
};

}  // namespace gui
}  // namespace delphyne

#endif
