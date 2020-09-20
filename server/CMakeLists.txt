file(GLOB_RECURSE src
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(2d_br_server ${src} ${CMAKE_SOURCE_DIR}/shared/Network.h)
target_include_directories(2d_br_server PRIVATE ${CMAKE_SOURCE_DIR}/shared)

target_link_libraries(2d_br_server yojimbo)