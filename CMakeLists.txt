cmake_minimum_required(VERSION 3.12.0)
cmake_policy(SET CMP0079 NEW)

add_compile_options(-D_GLIBCXX_USE_CXX11_ABI=1)

project(ni_grpc_device_server C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CreateVirtualEnvironment)

# Workaround for: https://bugs.chromium.org/p/boringssl/issues/detail?id=423
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
  set(CMAKE_SYSTEM_PROCESSOR "amd64")
endif()

#----------------------------------------------------------------------
# Use the grpc targets directly from this build, only when not cross-compiling.
#----------------------------------------------------------------------
if(CMAKE_CROSSCOMPILING)
  find_program(_PROTOBUF_PROTOC protoc)
  find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)

  if(NOT _GRPC_DEVICE_NILRT_LEGACY_TOOLCHAIN)
    find_package(gRPC REQUIRED)
    find_library(_REFLECTION grpc++_reflection)
    find_library(_GRPC_GRPCPP grpc++)
    find_library(_PROTOBUF_LIBPROTOBUF protobuf)
    # find_package(ni-grpc-sideband REQUIRED)
  else()
    add_subdirectory(third_party/grpc ${CMAKE_CURRENT_BINARY_DIR}/grpc EXCLUDE_FROM_ALL)
    set(_REFLECTION grpc++_reflection)
    set(_GRPC_GRPCPP grpc++)
    set(_PROTOBUF_LIBPROTOBUF libprotobuf)
    add_subdirectory(third_party/grpc-sideband ${CMAKE_CURRENT_BINARY_DIR}/grpc-sideband)
  endif()

else()
  add_subdirectory(third_party/grpc ${CMAKE_CURRENT_BINARY_DIR}/grpc EXCLUDE_FROM_ALL)
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
  set(_REFLECTION grpc++_reflection)
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
  set(_GRPC_GRPCPP grpc++)
  set(_PROTOBUF_LIBPROTOBUF libprotobuf)
  add_subdirectory(third_party/grpc-sideband ${CMAKE_CURRENT_BINARY_DIR}/grpc-sideband)
endif()

CreateVirtualEnvironment(virtual_environment
  REQUIREMENTS_TXT 
    ${CMAKE_SOURCE_DIR}/python_build_requirements.txt
  ENV_NAME
    venv
  OUT_PYTHON_EXE 
    PYTHON_EXE
)

#----------------------------------------------------------------------
# Use C++17 (needed for shared_mutex support on Linux)
#----------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#----------------------------------------------------------------------
# Include generated *.pb.h files
#----------------------------------------------------------------------
set(proto_srcs_dir "${CMAKE_CURRENT_BINARY_DIR}/proto")
file(MAKE_DIRECTORY ${proto_srcs_dir})
include_directories(
  "${proto_srcs_dir}"
  "./generated"
  "./imports/include"
  "./source"
  "./third_party/grpc-sideband/src"
)

#----------------------------------------------------------------------
# Define libraries to link
#----------------------------------------------------------------------
if(WIN32)
  link_directories("./imports/lib/win64")
else()
  # Add grpc-sideband deps here
  link_directories("./imports/lib/linux")
endif()

#if(NOT MSVC)
#  set(RDMA_LIB "libnirdma.so")
#else()
#  set(RDMA_LIB "rdma.lib")
#endif()

#----------------------------------------------------------------------
# Get list of NI Driver APIs and directories to generate from
#----------------------------------------------------------------------
set(metadata_dir ${CMAKE_SOURCE_DIR}/source/codegen/metadata)
set(service_output_dir ${CMAKE_SOURCE_DIR}/generated)
set(codegen_dir ${CMAKE_SOURCE_DIR}/source/codegen)
set(custom_dir ${CMAKE_SOURCE_DIR}/source/custom)
set(nidrivers "nidaqmx" "nidcpower" "nidigitalpattern" "nidmm" "nifake" "nifake_extension" "nifake_non_ivi" "nifgen" "nifpga" "nirfmxbluetooth" "nirfmxinstr" "nirfmxlte" "nirfmxnr" "nirfmxspecan" "nirfmxwlan" "nirfsa" "nirfsg" "niscope" "niswitch" "nisync" "nitclk" "nixnet" "nixnetsocket")

#----------------------------------------------------------------------
# Create NI Driver APIs proto and server files
#----------------------------------------------------------------------
set(codegen_scripts
  "${codegen_dir}/client_helpers.py"
  "${codegen_dir}/common_helpers.py"
  "${codegen_dir}/generate_service.py"
  "${codegen_dir}/generate_shared_service_files.py"
  "${codegen_dir}/service_helpers.py"
  "${codegen_dir}/proto_helpers.py"
  "${codegen_dir}/metadata_mutation.py"
  "${codegen_dir}/metadata_validation.py"
  "${codegen_dir}/template_helpers.py"
  "${codegen_dir}/templates/client.cpp.mako"
  "${codegen_dir}/templates/client.h.mako"
  "${codegen_dir}/templates/client_helpers.mako"
  "${codegen_dir}/templates/library_interface.h.mako"
  "${codegen_dir}/templates/library.cpp.mako"
  "${codegen_dir}/templates/library.h.mako"
  "${codegen_dir}/templates/mock_library.h.mako"
  "${codegen_dir}/templates/proto.mako"
  "${codegen_dir}/templates/proto_helpers.mako"
  "${codegen_dir}/templates/register_all_services.h.mako"
  "${codegen_dir}/templates/register_all_services.cpp.mako"  
  "${codegen_dir}/templates/service.cpp.mako"
  "${codegen_dir}/templates/service_helpers.mako"
  "${codegen_dir}/templates/service.h.mako"
  "${codegen_dir}/templates/service_registrar.h.mako"
  "${codegen_dir}/templates/service_registrar.cpp.mako"  
  )

# Populated in the api loop below.
set(nidriver_service_srcs "")
set(nidriver_client_srcs "")

# We'll codegen all drivers, but some are not included in the server build.
set(nidrivers_to_build ${nidrivers})
# Fake drivers are not in the server and are added manually to the UnitTestRunner build.
list(FILTER nidrivers_to_build EXCLUDE REGEX "^nifake.*")
if (NOT WIN32)
  # None of the RFmx drivers support Linux and building against the headers fails.
  # Exclude them on Linux.
  list(FILTER nidrivers_to_build EXCLUDE REGEX "^nirfmx.*")
endif()

set(all_codegen_dependencies "")

foreach(api ${nidrivers})
  set(codegen_dependencies
    "${metadata_dir}/${api}/attributes.py"
    "${metadata_dir}/${api}/attributes_addon.py"
    "${metadata_dir}/${api}/config.py"
    "${metadata_dir}/${api}/config_addon.py"
    "${metadata_dir}/${api}/enums.py"
    "${metadata_dir}/${api}/enums_addon.py"
    "${metadata_dir}/${api}/functions.py"
    "${metadata_dir}/${api}/functions_addon.py"
    "${metadata_dir}/${api}/__init__.py")
  if (EXISTS "${metadata_dir}/${api}/custom_proto.mako")
    set(codegen_dependencies ${codegen_dependencies} "${metadata_dir}/${api}/custom_proto.mako")
  endif()
  set(all_codegen_dependencies
    ${all_codegen_dependencies}
    ${codegen_dependencies}
  )

  set(output_files
    ${service_output_dir}/${api}/${api}_client.cpp
    ${service_output_dir}/${api}/${api}_client.h
    ${service_output_dir}/${api}/${api}_library_interface.h
    ${service_output_dir}/${api}/${api}_library.cpp
    ${service_output_dir}/${api}/${api}_library.h
    ${service_output_dir}/${api}/${api}_mock_library.h
    ${service_output_dir}/${api}/${api}.proto
    ${service_output_dir}/${api}/${api}_service.cpp
    ${service_output_dir}/${api}/${api}_service.h
    ${service_output_dir}/${api}/${api}_service_registrar.cpp
    ${service_output_dir}/${api}/${api}_service_registrar.h
  )
  set(gen_command COMMAND ${PYTHON_EXE} ${codegen_dir}/generate_service.py ${metadata_dir}/${api}/ -o ${service_output_dir}/)
  if (api IN_LIST nidrivers_to_build)
    set(nidriver_service_srcs
      ${nidriver_service_srcs}
      "${service_output_dir}/${api}/${api}_service.cpp"
      "${service_output_dir}/${api}/${api}_service_registrar.cpp"
      "${service_output_dir}/${api}/${api}_library.cpp"
      "${custom_dir}/${api}_service.custom.cpp")
    set(nidriver_client_srcs
      ${nidriver_client_srcs}
      "${service_output_dir}/${api}/${api}_client.cpp")
  endif()
  set(proto_dependencies ${codegen_dependencies} ${codegen_scripts} virtual_environment)
  add_custom_command(OUTPUT ${output_files}
    ${gen_command}
    COMMENT "Generating proto file and service for ${api}"
    DEPENDS ${proto_dependencies})
endforeach()

add_custom_command(
  OUTPUT 
    ${service_output_dir}/register_all_services.cpp
    ${service_output_dir}/register_all_services.h
  COMMAND 
    ${PYTHON_EXE} ${codegen_dir}/generate_shared_service_files.py ${metadata_dir}/ -o ${service_output_dir}/
  COMMENT 
    "Generating shared service files"
  DEPENDS
    ${all_codegen_dependencies}
    ${codegen_scripts} 
    virtual_environment
)

set(nidriver_service_srcs
    ${nidriver_service_srcs}
    ${service_output_dir}/register_all_services.cpp
)

#----------------------------------------------------------------------
# Proto file
#----------------------------------------------------------------------
get_filename_component(session_proto "source/protobuf/session.proto" ABSOLUTE)
get_filename_component(nidevice_proto "source/protobuf/nidevice.proto" ABSOLUTE)
get_filename_component(data_moniker_proto "source/protobuf/data_moniker.proto" ABSOLUTE)
get_filename_component(session_proto_path "${session_proto}" PATH)

#----------------------------------------------------------------------
# Generate sources from proto files
# Usage: GenerateGrpcSources(PROTO <path> OUTPUT <path>...)
#----------------------------------------------------------------------
function(GenerateGrpcSources)
  set(oneValueArgs PROTO)
  set(multiValueArgs OUTPUT)
  cmake_parse_arguments(GENERATE_ARGS "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  set(output_files "${GENERATE_ARGS_OUTPUT}")
  set(proto_file "${GENERATE_ARGS_PROTO}")
  get_filename_component(proto_path "${proto_file}" PATH)
  # Asssumption: all outputs are in the same directory: use the zero-th.
  list(GET output_files 0 proto_srcs)
  get_filename_component(proto_out_path "${proto_srcs}" PATH)
  add_custom_command(
    OUTPUT ${output_files}
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${proto_out_path}"
      --cpp_out "${proto_out_path}"
      -I "${proto_path}"
      -I ${CMAKE_SOURCE_DIR}/third_party/grpc/third_party/protobuf/src/
      -I ${CMAKE_SOURCE_DIR}/source/protobuf
      --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
      "${proto_file}"
    DEPENDS "${proto_file}" "${session_proto}")
endfunction()

set(session_proto_srcs "${proto_srcs_dir}/session.pb.cc")
set(session_proto_hdrs "${proto_srcs_dir}/session.pb.h")
set(session_grpc_srcs "${proto_srcs_dir}/session.grpc.pb.cc")
set(session_grpc_hdrs "${proto_srcs_dir}/session.grpc.pb.h")
set(nidevice_proto_srcs "${proto_srcs_dir}/nidevice.pb.cc")
set(nidevice_proto_hdrs "${proto_srcs_dir}/nidevice.pb.h")
set(data_moniker_proto_srcs "${proto_srcs_dir}/data_moniker.pb.cc")
set(data_moniker_proto_hdrs "${proto_srcs_dir}/data_moniker.pb.h")
set(data_moniker_grpc_srcs "${proto_srcs_dir}/data_moniker.grpc.pb.cc")
set(data_moniker_grpc_hdrs "${proto_srcs_dir}/data_moniker.grpc.pb.h")

GenerateGrpcSources(
  PROTO
    ${session_proto}
  OUTPUT
    "${session_proto_srcs}" 
    "${session_proto_hdrs}" 
    "${session_grpc_srcs}" 
    "${session_grpc_hdrs}"
)

GenerateGrpcSources(
  PROTO
    ${nidevice_proto}
  OUTPUT
    "${nidevice_proto_srcs}"
    "${nidevice_proto_hdrs}"
)

GenerateGrpcSources(
  PROTO
    ${data_moniker_proto}
  OUTPUT
    "${data_moniker_proto_srcs}"
    "${data_moniker_proto_hdrs}"
    "${data_moniker_grpc_srcs}"
    "${data_moniker_grpc_hdrs}"
)

foreach(api ${nidrivers})
  GenerateGrpcSources(
    PROTO
      ${service_output_dir}/${api}/${api}.proto
    OUTPUT
      "${proto_srcs_dir}/${api}.pb.cc"
      "${proto_srcs_dir}/${api}.pb.h"
      "${proto_srcs_dir}/${api}.grpc.pb.cc"
      "${proto_srcs_dir}/${api}.grpc.pb.h"
  )
  if(api IN_LIST nidrivers_to_build)
    set(nidriver_service_srcs
      ${nidriver_service_srcs}
      "${proto_srcs_dir}/${api}.pb.cc"
      "${proto_srcs_dir}/${api}.grpc.pb.cc")
  endif()
endforeach()

add_executable(ni_grpc_device_server
  "source/server/core_server.cpp"
  "source/server/core_service_registrar.cpp"
  "source/server/device_enumerator.cpp"
  "source/server/feature_toggles.cpp"
  "source/server/logging.cpp"
  "source/server/semaphore.cpp"
  "source/server/server_configuration_parser.cpp"
  "source/server/server_security_configuration.cpp"
  "source/server/session_repository.cpp"
  "source/server/session_utilities_service.cpp"
  "source/server/shared_library.cpp"
  "source/server/syscfg_library.cpp"
  "source/server/data_moniker_service.cpp"
  ${data_moniker_proto_srcs}
  ${data_moniker_grpc_srcs}
  ${nidevice_proto_srcs}
  ${session_proto_srcs}
  ${session_grpc_srcs}
  ${nidriver_service_srcs})

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
  target_sources(ni_grpc_device_server
    PRIVATE "source/server/linux/syslog_logging.cpp"
    PRIVATE "source/server/linux/daemonize.cpp")
endif()

target_link_libraries(ni_grpc_device_server
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  ${CMAKE_DL_LIBS}
  nlohmann_json::nlohmann_json
)

target_link_libraries(ni_grpc_device_server
  ni_grpc_sideband
  ${RDMA_LIB}
)

set_target_properties(ni_grpc_device_server PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)

if(WIN32)
  target_link_libraries(ni_grpc_device_server Delayimp nidaqmx nidcpower niDigital nidmm niFgen niScope niRFmxBT niRFmxInstr niRFmxLTE niRFmxNR niRFmxSpecAn niRFmxWLAN niRFSA niRFSG niswitch nisync niTClk nixnet nixntipstack)
  set_target_properties(ni_grpc_device_server PROPERTIES LINK_FLAGS "/DELAYLOAD:nicaiu.dll /DELAYLOAD:nidcpower.dll /DELAYLOAD:niDigital_64.dll /DELAYLOAD:nidmm_64.dll /DELAYLOAD:niFgen_64.dll /DELAYLOAD:niRFmxBT.dll /DELAYLOAD:niRFmxInstr.dll /DELAYLOAD:niRFmxLTE.dll /DELAYLOAD:niRFmxNR.dll /DELAYLOAD:niRFmxSpecAn.dll /DELAYLOAD:niRFmxWLAN.dll /DELAYLOAD:niRFSA_64.dll /DELAYLOAD:niRFSG_64.dll /DELAYLOAD:niScope.dll /DELAYLOAD:niswitch.dll /DELAYLOAD:nisync.dll /DELAYLOAD:niTClk_64.dll /DELAYLOAD:nixnet.dll /DELAYLOAD:nixntipstack.dll")
  set(ni_grpc_device_server CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:nicaiu.dll /DELAYLOAD:nidcpower.dll /DELAYLOAD:niDigital_64.dll /DELAYLOAD:nidmm_64.dll /DELAYLOAD:niFgen_64.dll /DELAYLOAD:niScope.dll /DELAYLOAD:niRFmxBT.dll /DELAYLOAD:niRFmxInstr.dll /DELAYLOAD:niRFmxLTE.dll /DELAYLOAD:niRFmxNR.dll /DELAYLOAD:niRFmxSpecAn.dll /DELAYLOAD:niRFmxWLAN.dll /DELAYLOAD:niRFSA_64.dll /DELAYLOAD:niRFSG_64.dll /DELAYLOAD:niswitch.dll /DELAYLOAD:nisync.dll /DELAYLOAD:niTClk_64.dll /DELAYLOAD:nixnet.dll /DELAYLOAD:nixntipstack.dll")
endif()

#----------------------------------------------------------------------
# Copy server_config.json to binary output directory
#----------------------------------------------------------------------
add_custom_command(
   TARGET ni_grpc_device_server POST_BUILD
   COMMAND  ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/source/config/insecure_config.json
            $<TARGET_FILE_DIR:ni_grpc_device_server>/server_config.json)

add_subdirectory(third_party/json ${CMAKE_CURRENT_BINARY_DIR}/json EXCLUDE_FROM_ALL)
