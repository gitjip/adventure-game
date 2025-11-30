cmake_minimum_required(VERSION 3.10)
project(adventure-game VERSION 0.1.0 LANGUAGES CXX)

# Use modern behavior for Boost package resolution (suppress CMP0167 dev warning)
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to enable the SceneLoader (useful to temporarily disable when toolchain
# or third-party headers are not available). Default ON so loader and its tests
# are built by default in developer environments where dependencies are available.
option(ENABLE_SCENELOADER "Enable SceneLoader and its tests" ON)

# Ensure nlohmann/json single-header is available in include/ (download if missing)
set(NLOHMANN_HEADER_DIR "${CMAKE_SOURCE_DIR}/include/nlohmann")
set(NLOHMANN_HEADER "${NLOHMANN_HEADER_DIR}/json.hpp")
if(NOT EXISTS "${NLOHMANN_HEADER}")
  file(MAKE_DIRECTORY "${NLOHMANN_HEADER_DIR}")
  message(STATUS "Downloading nlohmann/json single header to ${NLOHMANN_HEADER}")
  file(DOWNLOAD
    "https://raw.githubusercontent.com/nlohmann/json/v3.11.2/single_include/nlohmann/json.hpp"
    "${NLOHMANN_HEADER}"
    SHOW_PROGRESS
    STATUS _dl_status
    LOG log_download
  )
  list(GET _dl_status 0 _dl_code)
  if(NOT _dl_code EQUAL 0)
    message(WARNING "Failed to download nlohmann/json.hpp: ${log_download}")
  endif()
endif()

# Allow user to pass BOOST_ROOT if Boost is installed in a custom location
if(DEFINED ENV{BOOST_ROOT} AND NOT DEFINED BOOST_ROOT)
  set(BOOST_ROOT $ENV{BOOST_ROOT})
endif()

if(DEFINED BOOST_ROOT)
  set(_boost_inc "${BOOST_ROOT}/include")
  set(_boost_inc_alt "${BOOST_ROOT}/x86_64-w64-mingw32/include")
  set(_boost_libdir "${BOOST_ROOT}/lib")
  if(EXISTS "${_boost_inc_alt}")
    message(STATUS "Found alternative include path: ${_boost_inc_alt}")
    set(Boost_INCLUDE_DIRS "${_boost_inc_alt}")
    set(_boost_inc "${_boost_inc_alt}")
  elseif(EXISTS "${_boost_inc}")
    message(STATUS "Attempting manual Boost detection using BOOST_ROOT=${BOOST_ROOT}")
    set(Boost_INCLUDE_DIRS "${_boost_inc}")
  else()
    message(WARNING "BOOST_ROOT includes not found at ${_boost_inc} or ${_boost_inc_alt}")
  endif()

  if(EXISTS "${_boost_inc_alt}")
    message(STATUS "Using alternate boost include: ${_boost_inc_alt}")
    set(Boost_INCLUDE_DIRS "${_boost_inc_alt}")
  elseif(EXISTS "${_boost_inc}")
    message(STATUS "Using boost include: ${_boost_inc}")
    set(Boost_INCLUDE_DIRS "${_boost_inc}")
  else()
    message(WARNING "Neither ${_boost_inc_alt} nor ${_boost_inc} exists; manual detection will likely fail")
  endif()

  file(GLOB _fs_libs_x64
    "${_boost_libdir}/*boost_filesystem*x64*.a"
    "${_boost_libdir}/*boost_filesystem*x64*.lib"
    "${_boost_libdir}/*boost_filesystem*x64*.dll"
  )
  if(_fs_libs_x64)
    list(GET _fs_libs_x64 0 _fs_lib)
  else()
    file(GLOB _fs_libs
      "${_boost_libdir}/*boost_filesystem*.a"
      "${_boost_libdir}/*boost_filesystem*.lib"
      "${_boost_libdir}/*boost_filesystem*.dll"
    )
    if(_fs_libs)
      list(GET _fs_libs 0 _fs_lib)
    endif()
  endif()

  if(DEFINED _fs_lib)
    set(Boost_LIBRARIES "${_fs_lib}")
    set(Boost_FOUND TRUE)
    message(STATUS "Manual Boost found: include=${Boost_INCLUDE_DIRS} libs=${Boost_LIBRARIES}")
  else()
    message(WARNING "Manual Boost detection failed in ${_boost_libdir}. Found filesystem libs: ${_fs_libs_x64} ${_fs_libs}")
  endif()
endif()

if(NOT Boost_FOUND)
  find_package(Boost REQUIRED COMPONENTS filesystem json)
  if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found. Set BOOST_ROOT or install Boost.")
  endif()
endif()

message(STATUS "DEBUG: BOOST_ROOT=${BOOST_ROOT}")
message(STATUS "DEBUG: Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}")

if(DEFINED BOOST_ROOT)
  set(_boost_inc "${BOOST_ROOT}/include")
  if(EXISTS "${_boost_inc}")
    message(STATUS "Adding explicit include dir: ${_boost_inc}")
    include_directories(BEFORE SYSTEM "${_boost_inc}")
  else()
    message(WARNING "Expected boost include dir not found: ${_boost_inc}")
  endif()
endif()

# Application executable (only main)
add_executable(adventure_game
  src/main.cpp
)

# Core library with game implementation so tests can link to it
add_library(adventure_core STATIC
  src/Game.cpp
)

# Add system implementations (REQ-0002 scaffolding)
target_sources(adventure_core PRIVATE
  src/Systems.cpp
)

# Add scene/event implementations (REQ-0003)
target_sources(adventure_core PRIVATE
  src/Event.cpp
  src/Scene.cpp
)

if(ENABLE_SCENELOADER)
  target_sources(adventure_core PRIVATE
    src/SceneLoader.cpp
  )
else()
  message(STATUS "SceneLoader build disabled (ENABLE_SCENELOADER=OFF)")
endif()

target_include_directories(adventure_core PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(adventure_game PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Ensure Boost headers are available to the core target (json is header-only in many Boost versions)
if(DEFINED Boost_INCLUDE_DIRS)
  target_include_directories(adventure_core PUBLIC "${Boost_INCLUDE_DIRS}")
  message(STATUS "Added Boost include dirs to adventure_core: ${Boost_INCLUDE_DIRS}")
endif()

if(DEFINED BOOST_ROOT)
  if(EXISTS "${BOOST_ROOT}/include")
    target_include_directories(adventure_core PRIVATE "${BOOST_ROOT}/include")
    target_include_directories(adventure_game PRIVATE "${BOOST_ROOT}/include")
  endif()
  if(EXISTS "${BOOST_ROOT}/x86_64-w64-mingw32/include")
    target_include_directories(adventure_core PRIVATE "${BOOST_ROOT}/x86_64-w64-mingw32/include")
    target_include_directories(adventure_game PRIVATE "${BOOST_ROOT}/x86_64-w64-mingw32/include")
  endif()
endif()

if(TARGET Boost::filesystem AND TARGET Boost::system)
  target_link_libraries(adventure_core PRIVATE Boost::filesystem Boost::system)
else()
  target_link_libraries(adventure_core PRIVATE ${Boost_LIBRARIES})
endif()

target_link_libraries(adventure_game PRIVATE adventure_core)

message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

# Copy runtime data (levels, logs, etc.) into the build folder so tests and
# running the executable from the build directory can find assets.
add_custom_target(copy_data ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
  COMMENT "Copying data/ -> ${CMAKE_BINARY_DIR}/data"
)

# Option to build unit tests
option(ENABLE_TESTS "Build unit tests" ON)
if(ENABLE_TESTS)
  enable_testing()
  add_executable(unit_tests
    tests/test_game.cpp
  )
  target_include_directories(unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
  if(DEFINED BOOST_ROOT AND EXISTS "${BOOST_ROOT}/include")
    target_include_directories(unit_tests PRIVATE "${BOOST_ROOT}/include")
  endif()
  # Link core library into unit tests so Game symbols resolve
  target_link_libraries(unit_tests PRIVATE adventure_core)
  # Ensure test binary is built after data files are copied to the build dir
  add_dependencies(unit_tests copy_data)
  add_test(NAME unit_tests COMMAND unit_tests)

  # Separate test target for requirement REQ-0001 to avoid multiple 'main' definitions
  add_executable(unit_test_req_0001
    tests/test_req_0001.cpp
  )
  target_include_directories(unit_test_req_0001 PRIVATE ${CMAKE_SOURCE_DIR}/include)
  if(DEFINED BOOST_ROOT AND EXISTS "${BOOST_ROOT}/include")
    target_include_directories(unit_test_req_0001 PRIVATE "${BOOST_ROOT}/include")
  endif()
  target_link_libraries(unit_test_req_0001 PRIVATE adventure_core)
  add_dependencies(unit_test_req_0001 copy_data)
  add_test(NAME unit_req_0001 COMMAND unit_test_req_0001)
  
  # Survival system tests (REQ-0002)
  add_executable(unit_test_survival
    tests/test_survival.cpp
  )
  target_include_directories(unit_test_survival PRIVATE ${CMAKE_SOURCE_DIR}/include)
  if(DEFINED BOOST_ROOT AND EXISTS "${BOOST_ROOT}/include")
    target_include_directories(unit_test_survival PRIVATE "${BOOST_ROOT}/include")
  endif()
  target_link_libraries(unit_test_survival PRIVATE adventure_core)
  add_dependencies(unit_test_survival copy_data)
  add_test(NAME unit_survival COMMAND unit_test_survival)
  
  # Scene/Event tests (REQ-0003)
  add_executable(unit_test_scene_event
    tests/test_scene_event.cpp
  )
  target_include_directories(unit_test_scene_event PRIVATE ${CMAKE_SOURCE_DIR}/include)
  if(DEFINED BOOST_ROOT AND EXISTS "${BOOST_ROOT}/include")
    target_include_directories(unit_test_scene_event PRIVATE "${BOOST_ROOT}/include")
  endif()
  target_link_libraries(unit_test_scene_event PRIVATE adventure_core)
  add_dependencies(unit_test_scene_event copy_data)
  add_test(NAME unit_scene_event COMMAND unit_test_scene_event)

  # SceneLoader tests (REQ-0003 loader)
  if(ENABLE_SCENELOADER)
    add_executable(unit_test_scene_loader
      tests/test_scene_loader.cpp
    )
    target_include_directories(unit_test_scene_loader PRIVATE ${CMAKE_SOURCE_DIR}/include)
    if(DEFINED BOOST_ROOT AND EXISTS "${BOOST_ROOT}/include")
      target_include_directories(unit_test_scene_loader PRIVATE "${BOOST_ROOT}/include")
    endif()
    target_link_libraries(unit_test_scene_loader PRIVATE adventure_core)
    add_dependencies(unit_test_scene_loader copy_data)
    add_test(NAME unit_scene_loader COMMAND unit_test_scene_loader)
  else()
    message(STATUS "SceneLoader tests disabled (ENABLE_SCENELOADER=OFF)")
  endif()
endif()

