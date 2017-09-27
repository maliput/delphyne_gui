// Copyright 2017 Open Source Robotics Foundation
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

#ifndef DELPHYNE_BRIDGE_REPEATERFACTORY_HH_
#define DELPHYNE_BRIDGE_REPEATERFACTORY_HH_

#include <map>
#include <memory>
#include <string>

#include "ign_topic_repeater.hh"

namespace delphyne {
namespace bridge {

/// \brief Prototype for repeater factory generation
typedef std::shared_ptr<delphyne::bridge::AbstractRepeater> (
    *RepeaterFactoryFunction)(std::shared_ptr<lcm::LCM> lcm,
                              const std::string& topicName);

/// \brief A factory that creates repeater objects based on a string type
/// describing the origin type to repeat.
class RepeaterFactory {
 public:
  /// \brief Register a new repeater.
  /// \param[in] messageType Type of message to register.
  /// \param[in] factoryFunction Function that creates a new repeater.
  static void Register(const std::string& messageType,
                       RepeaterFactoryFunction factoryFunction);

  /// \brief Create a new repeater.
  /// \param[in] messageType Type of message this repeater will be translating.
  /// \param[in] lcm The LCM instance we are using to dispatch messages
  /// \param[in] topicName The name of the topic to repeat
  /// \return Pointer to a repeater. Null if the mapping can't be found.
  static std::shared_ptr<delphyne::bridge::AbstractRepeater> New(
      const std::string& messageType, std::shared_ptr<lcm::LCM> lcm,
      const std::string& topicName);
};

/// \brief Static message registration macro
///
/// Use this macro to register messages.
/// \param[in] messageType Message type name.
/// \param[in] ignitionType Class name for the ignition message.
/// \param[in] lcmType Class name for the lcm message.
/// \param[in] uid A unique id used to define the new function and class.
#define REGISTER_STATIC_REPEATER(messageType, ignitionType, lcmType, uid)      \
  std::shared_ptr<delphyne::bridge::AbstractRepeater>                          \
      DelpyneStaticRepeaterFactoryNew##uid(std::shared_ptr<lcm::LCM> lcm,      \
                                           const std::string& topicName) {     \
    return std::make_shared<                                                   \
        delphyne::bridge::IgnTopicRepeater<ignitionType, lcmType>>(lcm,        \
                                                                   topicName); \
  }                                                                            \
  class DelpyneStaticRepeaterClass##uid {                                      \
   public:                                                                     \
    DelpyneStaticRepeaterClass##uid() {                                        \
      delphyne::bridge::RepeaterFactory::Register(                             \
          messageType, DelpyneStaticRepeaterFactoryNew##uid);                  \
    }                                                                          \
  };                                                                           \
  static DelpyneStaticRepeaterClass##uid RepeaterInitializer;

}  // namespace bridge
}  // namespace delphyne

#endif