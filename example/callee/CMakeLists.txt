aux_source_directory(. SRC_LIST)
aux_source_directory(${CMAKE_SOURCE_DIR}/example PROTO_LIST)

find_package(Protobuf REQUIRED)

# 编译动态库
# add_library(MuduoSimplify SHARED ${SRC_LIST})
add_executable(provider ${SRC_LIST} ${PROTO_LIST})

target_link_libraries(provider PRIVATE spdlog::spdlog  protobuf::libprotobuf)
