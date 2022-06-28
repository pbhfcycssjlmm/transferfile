# Copyright 2018 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# cmake build file for C++ helloworld example.
# Assumes protobuf and gRPC have been installed using cmake.
# See cmake_externalproject/CMakeLists.txt for all-in-one cmake build
# that automatically builds all the dependencies before building helloworld.

cmake_minimum_required(VERSION 3.5.1)

project(Transferfile C CXX)

list(APPEND CMAKE_PREFIX_PATH "/home/$ENV{USER}/.local/bin")

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
  find_program(_PROTOBUF_PROTOC protoc)
else()
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()

# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)
if(CMAKE_CROSSCOMPILING)
  find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()

# Proto file
get_filename_component(tf_proto "../proto/transfer_file.proto" ABSOLUTE)
get_filename_component(tf_proto_path "${tf_proto}" PATH)

# Generated sources
set(tf_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/transfer_file.pb.cc")
set(tf_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/transfer_file.pb.h")
set(tf_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/transfer_file.grpc.pb.cc")
set(tf_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/transfer_file.grpc.pb.h")
add_custom_command(
      OUTPUT "${tf_proto_srcs}" "${tf_proto_hdrs}" "${tf_grpc_srcs}" "${tf_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${tf_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${tf_proto}"
      DEPENDS "${tf_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# tf_grpc_proto
add_library(tf_grpc_proto
  ${tf_grpc_srcs}
  ${tf_grpc_hdrs}
  ${tf_proto_srcs}
  ${tf_proto_hdrs})
target_link_libraries(tf_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# Targets greeter_[async_](client|server)
foreach(_target
  client server )
  add_executable(${_target} "${_target}.cc")
  target_link_libraries(${_target}
    tf_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
endforeach()
