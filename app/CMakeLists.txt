file(GLOB_RECURSE src
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_definitions(-DRESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")

add_executable(2d_br ${src} ${CMAKE_SOURCE_DIR}/shared/Network.h)
target_include_directories(2d_br PRIVATE ${CMAKE_SOURCE_DIR}/shared)

target_link_libraries(2d_br sfml-graphics sfml-window sfml-system yojimbo)

# Get rid of 'missing pdb' type of warnings when linking.
if (WIN32)
  set_target_properties(2d_br PROPERTIES LINK_FLAGS "/ignore:4099 /ignore:4098")
endif()