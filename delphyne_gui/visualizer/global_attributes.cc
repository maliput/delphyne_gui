// Copyright 2017 Toyota Research Institute

#include <regex>
#include <stdexcept>
#include <utility>
#include <vector>

#include <ignition/common/Console.hh>

#include "global_attributes.hh"

namespace delphyne {
namespace gui {
namespace {

// \brief Parses `arg` and checks all the constraints explained in
// `ParseArguments` documentation.
// \throws std::runtime_error When any of the explained conditions in
// `ParseArguments` documentation is met.
// \param[in] arg A string to parse.
// \returns A pair whose first value is the key, and the second is the value.
std::pair<std::string, std::string> ExtractKeyAndValue(const std::string& arg) {
  // 2 for "--" + 1+ for parameter name + 1 for "=" + 1+ for parameter value.
  if (arg.size() < 5) {
    ignerr << "Invalid argument length.\n";
    throw std::runtime_error("Invalid argument length.\n");
  }
  // Finds spaces, carry returns, new line or tabs in the string.
  const std::regex invalid_characters("[ \r|\n|\t]");
  std::smatch matcher;
  if (std::regex_search(arg, matcher, invalid_characters)) {
    ignerr << "Invalid characters found in: " << arg << "\n";
    throw std::runtime_error(std::string("Invalid characters found in: ") + arg + std::string("\n"));
  }

  // Checks that arg starts with "--".
  const std::string start_token("--");
  const auto start_it = arg.find(start_token);
  if (start_it != 0) {
    ignerr << "The argument: \"" << arg << "\" does not start with \"--\"\n";
    throw std::runtime_error(std::string("The argument: \"") + arg + std::string("\" does not start with \"--\"\n"));
  }

  // Checks that arg contains with "=" after "--".
  const std::string separator_token("=");
  const auto separator_it = arg.find(separator_token, 2);
  if (separator_it == std::string::npos) {
    ignerr << "The argument: \"" << arg << "\" does not contain \"=\"\n";
    throw std::runtime_error(std::string("The argument: \"") + arg + std::string("\" does not contain \"=\"\n"));
  }

  // Gets the key and the value.
  const std::string key = arg.substr(start_token.size(), separator_it - start_token.size());
  if (key.size() == 0) {
    ignerr << "Key: \"" << key << "\" does not contain at least 1 character.\n";
    throw std::runtime_error(std::string("Key: \"") + key + std::string("\" does not contain at least 1 character.\n"));
  }
  const std::string value = arg.substr(separator_it + separator_token.size());
  if (value.size() == 0) {
    ignerr << "Value: \"" << value << "\" does not contain at least 1 character.\n";
    throw std::runtime_error(std::string("Value: \"") + value +
                             std::string("\" does not contain at least 1 character.\n"));
  }
  return std::make_pair(key, value);
}

}  // namespace

std::map<std::string, std::string> GlobalAttributes::globalAttributes;

void GlobalAttributes::ParseArguments(int argc, const char* const* argv) {
  if (argc < 0) {
    ignerr << "argc should be positive.\n";
    throw std::runtime_error("argc should be positive.");
  }
  for (int i = 0; i < argc; ++i) {
    const std::pair<std::string, std::string> key_value = ExtractKeyAndValue(std::string(argv[i]));
    globalAttributes[key_value.first] = key_value.second;
  }
}

bool GlobalAttributes::HasArgument(const std::string& key) {
  return globalAttributes.find(key) != globalAttributes.end();
}

std::string GlobalAttributes::GetArgument(const std::string& key) {
  if (!HasArgument(key)) {
    ignerr << "Missing key: \"" << key << "\"\n";
    throw std::runtime_error(std::string("Missing key: \"") + key + std::string("\"\n"));
  }
  return globalAttributes[key];
}

}  // namespace gui
}  // namespace delphyne
