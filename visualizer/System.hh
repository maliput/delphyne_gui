// Copyright 2017 Toyota Research Institute

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
