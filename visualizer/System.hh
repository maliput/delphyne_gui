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

#ifndef DELPHYNE_GUI_SYSTEM_HH_
#define DELPHYNE_GUI_SYSTEM_HH_

#if defined(__GNUC__)
#define DELPHYNE_GUI_DEPRECATED(version) __attribute__((deprecated))
#define DELPHYNE_GUI_FORCEINLINE __attribute__((always_inline))
#elif defined(_WIN32)
#define DELPHYNE_GUI_DEPRECATED(version) ()
#define DELPHYNE_GUI_FORCEINLINE __forceinline
#else
#define DELPHYNE_GUI_DEPRECATED(version) ()
#define DELPHYNE_GUI_FORCEINLINE
#endif

/// \def DELPHYNE_GUI_VISIBLE
/// Use to represent "symbol visible" if supported

/// \def DELPHYNE_GUI_HIDDEN
/// Use to represent "symbol hidden" if supported
#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define DELPHYNE_GUI_VISIBLE __attribute__ ((dllexport))
    #else
      #define DELPHYNE_GUI_VISIBLE __declspec(dllexport)
    #endif
  #else
    #ifdef __GNUC__
      #define DELPHYNE_GUI_VISIBLE __attribute__ ((dllimport))
    #else
      #define DELPHYNE_GUI_VISIBLE __declspec(dllimport)
    #endif
  #endif
  #define DELPHYNE_GUI_HIDDEN
#else
  #if __GNUC__ >= 4
    #define DELPHYNE_GUI_VISIBLE __attribute__ ((visibility ("default")))
    #define DELPHYNE_GUI_HIDDEN  __attribute__ ((visibility ("hidden")))
  #else
    #define DELPHYNE_GUI_VISIBLE
    #define DELPHYNE_GUI_HIDDEN
  #endif
#endif

#endif
