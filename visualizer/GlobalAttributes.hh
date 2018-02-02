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

#ifndef GLOBAL_ATTRIBUTES_HH
#define GLOBAL_ATTRIBUTES_HH

#include <string>

#include "System.hh"

namespace delphyne {
namespace gui {
namespace global_attributes {

/// \brief Parses a list of CLI arguments and holds them as pairs key-value.
/// \details Each element of @p argv must be "--{KEY}={VALUE}". The key will
/// be KEY, and the value will be VALUE. Note that "--" and "=" are necessary.
/// Each element of @p argv should not have: white spaces, '\n', '\r', '\t'
/// characters and must be at least 5 characters long including "--" and "=".
/// \param[in] argc The number of elements in @p argv.
/// \param[in] argv A vector of string constants that hold the arguments.
/// \throws std::runtime_error When any item of @p argv contains: white spaces,
/// '\n', '\r' or '\t' characters.
/// \throws std::runtime_error When any item of @p argv does not start with
/// "--".
/// \throws std::runtime_error When any item of @p argv does not have "=" from
/// the third position on.
/// \throws std::runtime_error When key or value any item of @p argv are empty
/// strings.
DELPHYNE_GUI_VISIBLE
void ParseArguments(int argc, const char** argv);

/// \brief Query the @p key existence.
/// \param[in] key A string to index the value.
/// \return true When @p key exists, false otherwise.
DELPHYNE_GUI_VISIBLE
bool HasArgument(const std::string& key);

/// \brief Gets a CLI argument previously set using SetArgument.
/// \param[in] key A string to index the value.
/// \return A std::string with the argument.
/// \throws std::runtime_error When there is no value for @p key.
DELPHYNE_GUI_VISIBLE
std::string GetArgument(const std::string& key);

}
}
}

#endif
