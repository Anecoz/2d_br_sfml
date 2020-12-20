if (WIN32)
  # SFML
  set(SFML_DIR ${CMAKE_SOURCE_DIR}/contrib/win/SFML-2.5.1/lib/cmake/SFML)
  find_package(SFML COMPONENTS graphics window system)

  if (NOT SFML_FOUND)
    message(FATAL "SFML Not found!")
  endif()

  # yojimbo
  add_library(yojimbo STATIC IMPORTED REQUIRED)
  set_target_properties(yojimbo PROPERTIES IMPORTED_LOCATION_DEBUG ${CMAKE_CURRENT_LIST_DIR}/win/yojimbo/lib/Debug/yojimbo.lib)
  set_target_properties(yojimbo PROPERTIES IMPORTED_LOCATION_RELEASE ${CMAKE_CURRENT_LIST_DIR}/win/yojimbo/lib/Release/yojimbo.lib)
  target_include_directories(yojimbo INTERFACE ${CMAKE_CURRENT_LIST_DIR}/win/yojimbo/include)

else()
  message(FATAL "Contribs only exist for Win32, aborting!")
endif()