cmake_minimum_required(VERSION 3.10)
project(adventure-game VERSION 0.1.0 LANGUAGES CXX)

# Use modern behavior for Boost package resolution (suppress CMP0167 dev warning)
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Allow user to pass BOOST_ROOT if Boost is installed in a custom location
if(DEFINED ENV{BOOST_ROOT} AND NOT DEFINED BOOST_ROOT)
  set(BOOST_ROOT $ENV{BOOST_ROOT})
endif()

# Prefer the FindBoost module (fallback) for typical MinGW-built Boost
# Some Boost distributions install CMake "config" files per-component which
# can cause errors when components like boost_system are not available. To
# force using CMake's shipped FindBoost (which searches for libraries directly)
# disable Boost's own CMake config lookup.
## If the user provided BOOST_ROOT, prefer a simple manual detection using
## that prefix to avoid CMake's config-mode quirks. This will set
## `Boost_INCLUDE_DIRS` and `Boost_LIBRARIES` to absolute paths when found.
if(DEFINED BOOST_ROOT)
  # Mingw builds often install headers under a triplet-prefixed folder.
  set(_boost_inc "${BOOST_ROOT}/include")
  set(_boost_inc_alt "${BOOST_ROOT}/x86_64-w64-mingw32/include")
  set(_boost_libdir "${BOOST_ROOT}/lib")
  # Prefer triplet-prefixed include path used by MinGW-w64 packages, then fall back
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
  
  # Common alternative include layout used by many MinGW builds
  if(EXISTS "${_boost_inc_alt}")
    message(STATUS "Using alternate boost include: ${_boost_inc_alt}")
    set(Boost_INCLUDE_DIRS "${_boost_inc_alt}")
  elseif(EXISTS "${_boost_inc}")
    message(STATUS "Using boost include: ${_boost_inc}")
    set(Boost_INCLUDE_DIRS "${_boost_inc}")
  else()
    message(WARNING "Neither ${_boost_inc_alt} nor ${_boost_inc} exists; manual detection will likely fail")
  endif()

    # Prefer libraries built for x64 (many MinGW packages include both x32 and x64 variants)
    file(GLOB _fs_libs_x64
      "${_boost_libdir}/*boost_filesystem*x64*.a"
      "${_boost_libdir}/*boost_filesystem*x64*.lib"
      "${_boost_libdir}/*boost_filesystem*x64*.dll"
    )
    if(_fs_libs_x64)
      list(GET _fs_libs_x64 0 _fs_lib)
    else()
      # fallback to any filesystem library
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
      # use absolute file path for linking (filesystem only)
      set(Boost_LIBRARIES "${_fs_lib}")
      set(Boost_FOUND TRUE)
      message(STATUS "Manual Boost found: include=${Boost_INCLUDE_DIRS} libs=${Boost_LIBRARIES}")
    else()
      message(WARNING "Manual Boost detection failed in ${_boost_libdir}. Found filesystem libs: ${_fs_libs_x64} ${_fs_libs}")
    endif()
endif()

## If manual detection didn't succeed, fall back to CMake's FindBoost.
if(NOT Boost_FOUND)
  # request only filesystem; some MinGW Boost packages don't provide separate
  # boost_system config/library. Filesystem may work on its own.
  find_package(Boost REQUIRED COMPONENTS filesystem)
  if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found. Set BOOST_ROOT or install Boost.")
  endif()
endif()

## NOTE: don't blindly add `${Boost_INCLUDE_DIRS}` here because some
## detection paths can produce an incorrect/truncated value (seen as
## `D:/Users/damin`). We add `${BOOST_ROOT}/include` explicitly below
## when available which is more reliable for local MinGW Boost installs.

# Debug: print resolved Boost variables to help diagnose truncated include paths
message(STATUS "DEBUG: BOOST_ROOT=${BOOST_ROOT}")
message(STATUS "DEBUG: Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}")

## Ensure the BOOST_ROOT include directory is explicitly passed to the compiler
if(DEFINED BOOST_ROOT)
  set(_boost_inc "${BOOST_ROOT}/include")
  if(EXISTS "${_boost_inc}")
    message(STATUS "Adding explicit include dir: ${_boost_inc}")
    include_directories(BEFORE SYSTEM "${_boost_inc}")
  else()
    message(WARNING "Expected boost include dir not found: ${_boost_inc}")
  endif()
endif()

add_executable(adventure_game
  src/main.cpp
  src/Game.cpp
)

target_include_directories(adventure_game PRIVATE ${CMAKE_SOURCE_DIR}/include)
## If BOOST_ROOT is set and contains an include directory, add it explicitly
if(DEFINED BOOST_ROOT)
  if(EXISTS "${BOOST_ROOT}/include")
    target_include_directories(adventure_game PRIVATE "${BOOST_ROOT}/include")
  endif()
  # Also add common MinGW triplet include path if present
  if(EXISTS "${BOOST_ROOT}/x86_64-w64-mingw32/include")
    target_include_directories(adventure_game PRIVATE "${BOOST_ROOT}/x86_64-w64-mingw32/include")
  endif()
endif()
if(TARGET Boost::filesystem AND TARGET Boost::system)
  # Modern CMake: prefer imported targets if both components are available
  target_link_libraries(adventure_game PRIVATE Boost::filesystem Boost::system)
else()
  # Fall back to raw library paths provided in ${Boost_LIBRARIES}
  target_link_libraries(adventure_game PRIVATE ${Boost_LIBRARIES})
endif()

message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
