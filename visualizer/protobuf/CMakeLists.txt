include (${project_cmake_dir}/Utils.cmake)

set (msgs
  automotive_driving_command.proto
  robot_model_request.proto
)

set(PROTOBUF_IMPORT_DIRS ${IGNITION-MSGS_INCLUDE_DIRS})
PROTOBUF_GENERATE_CPP(PROTO_SRC PROTO_HEADER
  ${msgs}
)

add_library(delphyne_gui_protobuf_msgs STATIC ${PROTO_SRC})

target_include_directories(delphyne_gui_protobuf_msgs PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
