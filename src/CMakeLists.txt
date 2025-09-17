

aux_source_directory(. SRC_LIST)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
# 设置生成动态库
add_library(mprpc SHARED ${SRC_LIST})


set_property(TARGET mprpc PROPERTY POSITION_INDEPENDENT_CODE ON)
# 动态库  链接 Muduo 库   target_link_libraries 是用来链接编译目标和
target_link_libraries(mprpc PRIVATE  spdlog::spdlog)      
