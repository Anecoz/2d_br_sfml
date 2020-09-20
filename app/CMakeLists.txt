file(GLOB_RECURSE src
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(2d_br ${src})

target_link_libraries(2d_br sfml-graphics sfml-window sfml-system yojimbo)