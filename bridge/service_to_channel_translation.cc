#include "service_to_channel_translation.hh"

namespace delphyne {
namespace bridge {

/// \brief Generates an LCM message aimed to  
/// tell drake that the visualizer is ready
drake::lcmt_viewer_command convertServiceToMsg(ignition::msgs::Empty _req) {
  // Create an empty lcm viewer command message
  drake::lcmt_viewer_command viewerCommand;
  // Hardcode content read by drake
  viewerCommand.command_type = drake::lcmt_viewer_command::STATUS;
  viewerCommand.command_data = "successfully loaded robot";
  return viewerCommand;
}

}  // namespace bridge
}  // namespace delphyne
