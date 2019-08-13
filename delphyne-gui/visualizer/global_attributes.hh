// Copyright 2017 Toyota Research Institute

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
  /// Each element of @p argv should not have: white spaces, '\n', '\r', '\t'
  /// characters and must be at least 5 characters long including "--" and "=".
  /// \param[in] argc The number of elements in @p argv.
  /// \param[in] argv A vector of string constants that hold the arguments.
  /// \throws std::runtime_error When any item of @p argv contains: white
  /// spaces, '\n', '\r' or '\t' characters.
  /// \throws std::runtime_error When any item of @p argv does not start with
  /// "--".
  /// \throws std::runtime_error When any item of @p argv does not have "=" from
  /// the third position on.
  /// \throws std::runtime_error When key or value any item of @p argv are empty
  /// strings.
 public:
  static void ParseArguments(int argc, const char** argv);

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
