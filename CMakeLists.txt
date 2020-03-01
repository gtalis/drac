cmake_minimum_required(VERSION 3.10)

project(libdrac)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# sources
file(GLOB sources
	engine/src/CardGraphics.cpp
	engine/src/CCardRegion.cpp
	engine/src/CCardStack.cpp
	engine/src/CGame.cpp
	engine/src/CCard.cpp
)

#-------------------------------------------------------------------------------
# Dependencies
#-------------------------------------------------------------------------------
include(FindPkgConfig)
# SDL
PKG_SEARCH_MODULE(SDL REQUIRED sdl)
# SDL_ttf
PKG_SEARCH_MODULE(SDL_TTF REQUIRED SDL_ttf)

include_directories(${SDL_INCLUDE_DIRS} ${SDL_TTF_INCLUDE_DIRS})

add_library(libdrac SHARED ${sources})
target_include_directories(libdrac PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/engine/includes"
                          )

target_link_libraries(libdrac ${SDL_LIBRARIES} ${SDL_TTF_LIBRARIES})

add_executable(klondike examples/klondike.cpp font/font.cpp)
target_link_libraries(klondike libdrac)

add_executable(bataille examples/bataille.cpp font/font.cpp)
target_link_libraries(bataille libdrac)
