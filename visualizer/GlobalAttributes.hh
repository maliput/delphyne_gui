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

/// \brief Adds a CLI argument and holds it as a std::string.
/// \param[in] argument A pointer to the string argument.
/// \return The index of @p argument. It is needed to retrieve it later.
DELPHYNE_GUI_VISIBLE
int SetArgument(const char* argument);

/// \brief Gets a CLI argument previously set using SetArgument.
/// \param[in] index An index to get a previously loaded argument. It must be
/// higher or equal to zero and smaller than the total number of arguments.
/// \see GetNumberOfArguments.
/// \return A std::string with the argument.
/// \throws std::runtime_error When index is negative or it is higher or equal
/// to the total number of arguments.
DELPHYNE_GUI_VISIBLE
std::string GetArgument(const int index);

/// \brief Gets the total number of loaded arguments.
/// \return The number of loaded arguments.
DELPHYNE_GUI_VISIBLE
int GetNumberOfArguments();

}
}
}

#endif
