# Build type link flags
set (CMAKE_LINK_FLAGS_RELEASE " " CACHE INTERNAL "Link flags for release" FORCE)
set (CMAKE_LINK_FLAGS_RELWITHDEBINFO " " CACHE INTERNAL "Link flags for release with debug support" FORCE)
set (CMAKE_LINK_FLAGS_DEBUG " " CACHE INTERNAL "Link flags for debug" FORCE)
set (CMAKE_LINK_FLAGS_PROFILE " -pg" CACHE INTERNAL "Link flags for profile" FORCE)

set (CMAKE_C_FLAGS_RELEASE "")
if (NOT APPLE)
  # -s doesn't work with default osx compiler clang, alternative:
  # http://stackoverflow.com/questions/6085491/gcc-vs-clang-symbol-strippingu
  set (CMAKE_C_FLAGS_RELEASE "-s")
endif()


set (CMAKE_C_FLAGS_RELEASE " ${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG" CACHE INTERNAL "C Flags for release" FORCE)
set (CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})

set (CMAKE_C_FLAGS_RELWITHDEBINFO " -g -O2" CACHE INTERNAL "C Flags for release with debug support" FORCE)
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO})

set (CMAKE_C_FLAGS_DEBUG " -ggdb3" CACHE INTERNAL "C Flags for debug" FORCE)
set (CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})

set (CMAKE_C_FLAGS_PROFILE " -fno-omit-frame-pointer -g -pg" CACHE INTERNAL "C Flags for profile" FORCE)
set (CMAKE_CXX_FLAGS_PROFILE ${CMAKE_C_FLAGS_PROFILE})

#####################################
# Set all the global build flags
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_UPPERCASE}}")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINK_FLAGS_${CMAKE_BUILD_TYPE_UPPERCASE}}")
set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_LINK_FLAGS_${CMAKE_BUILD_TYPE_UPPERCASE}}")
set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_LINK_FLAGS_${CMAKE_BUILD_TYPE_UPPERCASE}}")

if (UNIX)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_UPPERCASE}}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINK_FLAGS_${CMAKE_BUILD_TYPE_UPPERCASE}}")
    set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_LINK_FLAGS_${CMAKE_BUILD_TYPE_UPPERCASE}}")
    set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_LINK_FLAGS_${CMAKE_BUILD_TYPE_UPPERCASE}}")
endif()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UPPERCASE}} -std=c++17")

# C++ Version
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific C++17 activation.
if ("${CMAKE_CXX_COMPILER_ID} " MATCHES "GNU ")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (NOT (GCC_VERSION VERSION_GREATER 6.9))
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 7.0 or greater.")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdata-sections -fdiagnostics-color=always -ffunction-sections -fopenmp -fPIC   -fstack-protector -fno-omit-frame-pointer -no-canonical-prefixes -O2 -std=c++17 -Wall -Wno-builtin-macro-redefined -Wno-missing-field-initializers -Wregister -Wstrict-overflow -Wno-unused-const-variable")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID} " MATCHES "Clang ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdata-sections -fdiagnostics-color=always -ffunction-sections -fPIC -fstack-protector -fno-omit-frame-pointer -no-canonical-prefixes -O2 -std=c++17 -Wall -Wno-builtin-macro-redefined -Wno-deprecated-dynamic-exception-spec -Wno-enum-compare-switch -Wno-gnu-designator -Wno-missing-field-initializers -Wno-register -Wno-strict-overflow -Wno-unknown-warning-option -Wno-unneeded-internal-declaration -Wno-unused-const-variable")
elseif ("${CMAKE_CXX_COMPILER_ID} " STREQUAL "MSVC ")
    if (NOT MSVC12)
        message(FATAL_ERROR "${PROJECT_NAME} requires VS 2013 os greater.")
    endif()
else ()
    message(FATAL_ERROR "Your C++ compiler does not support C++17.")
endif ()
