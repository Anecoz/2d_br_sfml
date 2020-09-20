# SFML
set(SFML_DIR ${CMAKE_SOURCE_DIR}/contrib/SFML-2.5.1/lib/cmake/SFML)
find_package(SFML COMPONENTS graphics window system)

if (NOT SFML_FOUND)
  message(FATAL "SFML Not found!")
endif()

# yojimbo
add_library(yojimbo STATIC IMPORTED REQUIRED)
set_target_properties(yojimbo PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/yojimbo/lib/Release/yojimbo.lib)
#set_target_properties(yojimbo PROPERTIES IMPORTED_LOCATION_Release ${CMAKE_CURRENT_LIST_DIR}/yojimbo/lib/Release/yojimbo.lib)
target_include_directories(yojimbo INTERFACE ${CMAKE_CURRENT_LIST_DIR}/yojimbo/include)