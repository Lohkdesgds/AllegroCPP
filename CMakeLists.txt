cmake_minimum_required(VERSION 3.28)

project(AllegroCPP LANGUAGES CXX)

# ==== Allegro + Global Settings ==== #
set(WANT_TESTS OFF CACHE BOOL "" FORCE)
set(WANT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(WANT_DEMO OFF CACHE BOOL "" FORCE)
set(WANT_TOOLS OFF CACHE BOOL "" FORCE)
set(WANT_DOCS OFF CACHE BOOL "" FORCE)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SHARED OFF CACHE BOOL "" FORCE)
set(ALLEGRO_MONOLITH OFF CACHE BOOL "" FORCE)
set(WANT_STATIC ON CACHE BOOL "" FORCE)
set(WANT_SHARED OFF CACHE BOOL "" FORCE)

set(WANT_IMAGE ON CACHE BOOL "" FORCE)
set(WANT_PRIMITIVES ON CACHE BOOL "" FORCE)
set(WANT_COLOR ON CACHE BOOL "" FORCE)
set(WANT_FONT ON CACHE BOOL "" FORCE)
set(WANT_TTF ON CACHE BOOL "" FORCE)
set(WANT_AUDIO ON CACHE BOOL "" FORCE)
set(WANT_ACODEC ON CACHE BOOL "" FORCE)
set(WANT_MEMFILE ON CACHE BOOL "" FORCE)
set(WANT_PHYSFS ON CACHE BOOL "" FORCE)
set(WANT_NATIVE_DIALOG ON CACHE BOOL "" FORCE)

include(FetchContent)
message(STATUS "Fetching Allegro 5...")

FetchContent_Declare(
  allegro5
  GIT_REPOSITORY https://github.com/liballeg/allegro5.git
  GIT_TAG        5.2.10.1
)

# On MSVC we prefer shared builds to avoid CRT linkage issues
if (MSVC)
  set(SHARED ON)
else()
  set(SHARED OFF)
endif()

FetchContent_MakeAvailable(allegro5)

# ==== Collect Allegro include paths ==== #
file(GLOB ALLEGRO_ADDON_DIRS ${allegro5_SOURCE_DIR}/addons/*)

set(ALLEGRO_ADDON_INCLUDE_DIRS "")
foreach(full_path ${ALLEGRO_ADDON_DIRS})
    if(IS_DIRECTORY "${full_path}")
        list(APPEND ALLEGRO_ADDON_INCLUDE_DIRS "${full_path}")
        message(STATUS "--> add Allegro path: ${full_path}")
    endif()
endforeach()

message(STATUS "--> add Allegro path: ${allegro5_SOURCE_DIR}/include")
list(APPEND ALLEGRO_ADDON_INCLUDE_DIRS "${allegro5_SOURCE_DIR}/include")

message(STATUS "--> add Allegro path: ${allegro5_BINARY_DIR}/include")
list(APPEND ALLEGRO_ADDON_INCLUDE_DIRS "${allegro5_BINARY_DIR}/include")

list(REMOVE_DUPLICATES ALLEGRO_ADDON_INCLUDE_DIRS)

# ==== Define your library ==== #
file(GLOB_RECURSE MAIN_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(
    AllegroCPP
    ${MAIN_SRCS}
)

target_include_directories(AllegroCPP
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${ALLEGRO_ADDON_INCLUDE_DIRS}
)

# Link Allegro targets - they are exported by the Allegro build
target_link_libraries(AllegroCPP
    PUBLIC
        allegro
        allegro_main
        allegro_image
        allegro_primitives
        allegro_dialog
        allegro_color
        allegro_font
        allegro_ttf
        allegro_audio
        allegro_acodec
        allegro_memfile
        allegro_physfs
)
